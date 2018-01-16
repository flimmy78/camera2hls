#include <kugou/httpproxy/liveencoder/encoder/Heartbeat.h>
#include <kugou/httpproxy/liveencoder/bean/AddEncodeChannelRequest.h>
#include <kugou/httpproxy/liveencoder/bean/AddEncodeChannelResponse.h>
#include <kugou/httpproxy/liveencoder/bean/DelEncodeChannelRequest.h>
#include <kugou/httpproxy/liveencoder/bean/DelEncodeChannelResponse.h>
#include <kugou/httpproxy/liveencoder/scheduleencoder/ScheduleEncoderLibrary.h>
#include <kugou/base/facility/log.h>
#include <kugou/httpproxy/liveencoder/SystemConfig.h>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <Poco/LocalDateTime.h>

#define MIN_CPU_USAGE 80

#ifdef _MSC_VER
std::string Archives_File = "\\archive\\";
#elif defined __GNUC__
std::string Archives_File = "/archive/";
#endif

using namespace kugou::base::net;
using namespace kugou::httpproxy::liveencoder::bean;
using namespace kugou::base::facility;

namespace kugou
{
namespace httpproxy
{
namespace liveencoder
{
namespace scheduleencoder
{

ScheduleEncoderLibrary::ScheduleEncoderLibrary(void)
    : m_isHeartbeatTimeoutThreadStop(false)
{
    m_heartbeatTimeoutThread.start(*this);
}

ScheduleEncoderLibrary::~ScheduleEncoderLibrary()
{
    m_isHeartbeatTimeoutThreadStop = true;
    m_heartbeatTimeoutThread.join();
}

void ScheduleEncoderLibrary::run()
{
    while(!m_isHeartbeatTimeoutThreadStop)
    {
        {
            boost::mutex::scoped_lock scopedLock(m_mutexEncoderUsageMap);
            EncoderMap::iterator it = m_encoderMap.begin();
            while(it != m_encoderMap.end())
            {
                // 2����û�����������ı�������Ҫɾ����
                if(it->second.m_updateTimestamp.elapsed() > 120000000)   // 2����
                {
                    LOG_W("Encoder [" << it->first << "] is out of connected after 2 miniute, delete it.");                      

                    cleanEncoderStreamID(it->first);
                    m_encoderMap.erase(it++); 
                }
                else
                {
                    ++it;
                }
            }            
        }

        Poco::Thread::sleep(5000); // 5����ټ��
    }
}

void ScheduleEncoderLibrary::cleanEncoderStreamID(std::string encoderID)
{
    boost::mutex::scoped_lock scopedLock(m_mutexStreamIDEncoderMAP);
    StreamIDEncoderMAP::iterator it = m_streamIDEncoderMAP.begin();
    while(it != m_streamIDEncoderMAP.end())
    {
        if(it->second == encoderID)
        {
            m_streamIDEncoderMAP.erase(it++);
        }
        else
        {
            ++it;
        }
    }
}

void ScheduleEncoderLibrary::addEncoder(std::string encoderID, std::vector<std::string>& idVector)
{
    boost::mutex::scoped_lock scopedLock(m_mutexEncoderUsageMap);

    EncoderMap::iterator it = m_encoderMap.find(encoderID);

    if(it == m_encoderMap.end())
    {
        EncoderInfo info;
        m_encoderMap[encoderID] = info;
    }

    // ������EC�м�¼�����·�һ��Stream ID����
    std::string newEncoderID = encoderID;
    std::size_t pos = newEncoderID.find_first_of(":",0);
    if(pos > 0)
    {
        newEncoderID.replace(pos,1, "-");
    }

    std::string currentPath = boost::filesystem::initial_path<boost::filesystem::path>().string();
    boost::filesystem::path archivePath(currentPath.append(Archives_File).append(newEncoderID));

    EncoderHeartbeatRequest req;
    if(req.fromJsonFile(archivePath.string()))
    {
        // �������EC֮ǰ��¼��ID��������ID�Ѿ��ڱ����ˣ��Ͳ��ύ��EC
        boost::mutex::scoped_lock scopedLock(m_mutexStreamIDEncoderMAP);
        std::vector<std::string>::iterator itBegin = req.StreamIDList.begin();
        for(;itBegin != req.StreamIDList.end(); itBegin++)
        {
            if(m_streamIDEncoderMAP.find(*itBegin) == m_streamIDEncoderMAP.end())
            {
                idVector.push_back(*itBegin);
                LOG_E("add Stream ID : " <<  *itBegin << ".");
            }
        }
    }
    else
    {
        LOG_E("Parse archive file : " <<  archivePath.string() << " to Json Error.");
    }
    
    LOG_I("Encoder [" << encoderID << "] register.");
}

void ScheduleEncoderLibrary::delEncoder(std::string encoderID)
{
    boost::mutex::scoped_lock scopedLock(m_mutexEncoderUsageMap);

    EncoderMap::iterator it = m_encoderMap.find(encoderID);

    if(it != m_encoderMap.end())
    {
        cleanEncoderStreamID(it->first);
        m_encoderMap.erase(it);
    }

    LOG_I("Encoder [" << encoderID << "] unregister.");
}

void ScheduleEncoderLibrary::onHeartbeat(EncoderHeartbeatRequest& request)
{
    {
        // ����CPU������
        // �����Ƿ��ҵ���������������Ƕ�Ҫ�����µ�CPU������
        // û���ҵ������������������ʧȥ���������������������ˣ����°����ӽ���
        boost::mutex::scoped_lock scopedLock(m_mutexEncoderUsageMap);
        EncoderInfo info(request.Usage_CPU, request.StreamIDList.size());
        m_encoderMap[request.Encoder_ID] = info;
    }

#if 0
    // ��ӡ���������������������ڱ������ID
    std::string streamIDList;
    std::vector< boost::shared_ptr<std::string> >::iterator it = request.StreamIDList.begin();
    while(it != request.StreamIDList.end())
    {
        streamIDList.append((*(*it)) + ",");
        ++it;
    }

    LOG_T("Encoder [" << (*request.Encoder_ID) << "] heartbeat with CPU usage [" << (*request.Usage_CPU)\
        << "], streamid size is [" << request.StreamIDList.size() << "] detail : " << streamIDList);
#endif

    // ����stream ID list ���µ�Set; 
    std::set<std::string> newStreamIDSet;
    std::vector<std::string>::iterator it = request.StreamIDList.begin();
    for(; it != request.StreamIDList.end(); it++)
    {
        newStreamIDSet.insert(*it);
    }

    {
        boost::mutex::scoped_lock scopedLock(m_mutexStreamIDEncoderMAP);
        StreamIDEncoderMAP::iterator it = m_streamIDEncoderMAP.begin();
        while(it != m_streamIDEncoderMAP.end())
        {
            // �鿴���Stream ID ��EC���Ƿ����
            // �����ھ�ɾ��
            if(it->second == request.Encoder_ID && newStreamIDSet.find(it->first) == newStreamIDSet.end())
            {
                LOG_I("Stream ID [" <<it->first << "] is no longer work in EC [" << it->second << "], so erase it.");
                m_streamIDEncoderMAP.erase(it++);
            }
            else
            {
                ++it;
            }
        }
    }

    std::string saveJsonFileName = "archive/" + (request.Encoder_ID);
    std::size_t pos = saveJsonFileName.find_first_of(":",0);
    if(pos > 0)
    {
        saveJsonFileName.replace(pos,1, "-");
    }
    if(!request.toJson(saveJsonFileName))
    {
        LOG_W("save json file[" << saveJsonFileName << "] error.");
    }
}

void ScheduleEncoderLibrary::addEncodeStreamIDSuccess(std::string id)
{
    LOG_I("EC report stream ID [" << id << "] lanch successful.");

    // ��WEB��̨�ӿڷ��ͱ��뿪ʼ֪ͨ
    HTTPClient client("127.0.0.1", "80", "/streamInterface.php?cmd=lowStr&streamName=" + id + "&encoded=1");
    std::string domin = SystemConfig::Singleton.get()->m_webBackendDomin;
    if(client.HTTPGet(domin))
    {
        //���ͳɹ�
        LOG_I("Report to WEB backend successful.");
    }
    else
    {
        //����ʧ��
        LOG_I("Report to WEB backend failed.");
    }
}

bool ScheduleEncoderLibrary::addEncodeStreamID(std::string id, std::string encoderID)
{
    int usage = MIN_CPU_USAGE;

    // ����Ǹ��������Ͳ��ò�����
    if(encoderID.empty())
    {
    // ��������ID�Ѿ���ӹ��ˣ��Ͳ��������
    {
        boost::mutex::scoped_lock scopedLock(m_mutexStreamIDEncoderMAP);
        StreamIDEncoderMAP::iterator it = m_streamIDEncoderMAP.find(id);
        if(it != m_streamIDEncoderMAP.end())
        {
            LOG_I("Encode stream ID [" << id << "] has been add before.");
            return false;
        }
    }
    
    // TODO ���ȿ������������ٵı��������
    // �ҵ�cpu������͵ı�����
    {
        boost::mutex::scoped_lock scopedLock(m_mutexEncoderUsageMap);
        EncoderMap::iterator it = m_encoderMap.begin();
        while(it != m_encoderMap.end())
        {
            if(it->second.m_cpuUsage < usage && it->second.m_tasks < MAX_TASK)
            {
                encoderID = it->first;
                usage = it->second.m_cpuUsage;
            }
            ++it;
        }
        
        // ��ֹ������ӱ�������ʱ��m_tasks���ܷ�ӳencoder�����������������ȼ��ϡ�
        EncoderMap::iterator findIt = m_encoderMap.find(encoderID);
        if(findIt != m_encoderMap.end())
        {
            findIt->second.m_tasks++;
        }
    }
    }

    // ����ҵ�CPU����MIN_CPU_USAGE�ı��������ַ���������˱�����
    if(!encoderID.empty())
    {
        AddChannelRequest request;
        std::string requestJson;
        std::string responseJson;  
        //AddChannelResponse response;
        std::size_t pos = encoderID.find_first_of(":");
        HTTPClient client(encoderID.substr(0, pos), encoderID.substr(pos + 1), ADDCHANNELREQUEST_API);
        request.Source_ID = id;

        boost::mutex::scoped_lock scopedLock(m_mutexStreamIDEncoderMAP);
        m_streamIDEncoderMAP.insert(std::make_pair(id, encoderID));
        scopedLock.unlock();

        requestJson = request.toJson();
        if(client.HTTPGet(requestJson, responseJson))// && response.Result.get() != NULL && (*response.Result) == "YES")
        {
            //���ͳɹ�
            LOG_I("Add encode stream ID [" << id << "] to encoder [" << encoderID << "] successful.");
        }
        else
        {
            //����ʧ��
            LOG_E("Add encode stream ID [" << id << "] to encoder [" << encoderID << "] failed.");
            return false;
        }
    }
    else
    {
        // TODO �����ַܷ�����
        LOG_W("all encoder's CPU are over " << MIN_CPU_USAGE << "%");
        return false;
    }    

    return true;
}

void ScheduleEncoderLibrary::delEncodeStreamID(std::string id)
{
    LOG_I("Del encode stream ID [" << id << "].");
    boost::mutex::scoped_lock scopedLock(m_mutexStreamIDEncoderMAP);

    StreamIDEncoderMAP::iterator it = m_streamIDEncoderMAP.find(id);

    if(it != m_streamIDEncoderMAP.end())
    {
        std::string encoderID = it->second;
        //����ҵ���������ʹӱ�������ɾ��
        DelChannelRequest request;
        std::string requestJson;
        std::string responseJson;  
        //DelChannelResponse response;
        std::size_t pos = encoderID.find_first_of(":");
        HTTPClient client(encoderID.substr(0, pos), encoderID.substr(pos + 1), DELCHANNELREQUEST_API);
        request.Source_ID = id;

        requestJson = request.toJson();
        if(client.HTTPGet(requestJson, responseJson))//&& response.Result.get() != NULL && (*response.Result) == "YES")
        {
            //���ͳɹ�
            LOG_I("Del encode stream ID [" << id << "] to encoder [" << encoderID << "] successful.");
            m_streamIDEncoderMAP.erase(it);
        }
        else
        {
            //����ʧ��
            LOG_E("Del encode stream ID [" << id << "] to encoder [" << encoderID << "] failed.");
        }
    }
}

}
}
}
}
