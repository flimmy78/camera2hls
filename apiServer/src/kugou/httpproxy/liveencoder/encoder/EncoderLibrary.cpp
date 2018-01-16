#include <fstream>

#include <kugou/httpproxy/liveencoder/encoder/EncoderLibrary.h>
#include <kugou/base/facility/log.h>

namespace kugou
{
namespace httpproxy
{
namespace liveencoder
{
namespace encoder
{

EncoderLibrary::EncoderLibrary(Poco::ThreadPool& tp, std::string& src, std::string& dst)
    : m_threadPool(tp)
    , m_srcStreamServer(src)
    , m_dstStreamServer(dst)
{
}

void EncoderLibrary::getStreamIDs(std::vector<std::string>& idVector)
{
    boost::mutex::scoped_lock scopedLock(m_mutexEncoderWorkerMap);
    EncoderWorkerMAP::iterator it = m_encoderWorkerMap.begin();

    while(it != m_encoderWorkerMap.end())
    {
        if(it->second->isStop())
        {
            // 在心跳遍历所有worker时，看这个worker是否已经停止，然后把它删除

            LOG_I("Stream ID [" << it->first << "] is finish, so erase it.");
            EncoderWorker *ew = it->second;
            m_encoderWorkerMap.erase(it++);
            delete ew;
        }
        else
        {
            idVector.push_back(it->first);
            ++it;
        }

    }
}

bool EncoderLibrary::addEncodeStreamID(std::string id, std::string encoderID)
{
    LOG_I("Add Stream ID [" << id << "].");
    boost::mutex::scoped_lock scopedLock(m_mutexEncoderWorkerMap);

    EncoderWorkerMAP::iterator it = m_encoderWorkerMap.find(id);

    if(it == m_encoderWorkerMap.end())
    {
        EncoderWorker *ew = new EncoderWorker(id, m_srcStreamServer, m_dstStreamServer);
        m_encoderWorkerMap[id] = ew;

        LOG_I("Find Stream ID [" << id << "], and start it.");
        m_threadPool.start(*ew);
    }
    return true;
}

void EncoderLibrary::delEncodeStreamID(std::string id)
{
    LOG_I("Delete Stream ID [" << id << "].");
    boost::mutex::scoped_lock scopedLock(m_mutexEncoderWorkerMap);

    EncoderWorkerMAP::iterator it = m_encoderWorkerMap.find(id);

    if(it != m_encoderWorkerMap.end())
    {
        // stop 函数，只是通知worker的线程退出
        LOG_I("Find Stream ID [" << id << "], and going to stop it.");
        it->second->stop();
    }

}

}
}
}
}
