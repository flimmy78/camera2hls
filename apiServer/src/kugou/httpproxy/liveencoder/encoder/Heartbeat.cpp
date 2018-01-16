#include <kugou/httpproxy/liveencoder/encoder/Heartbeat.h>
#include <kugou/base/facility/log.h>
#include <kugou/httpproxy/liveencoder/bean/EncoderHeartbeatRequest.h>
#include <kugou/httpproxy/liveencoder/bean/EncoderHeartbeatResponse.h>

#include <Poco/Thread.h>

#define HEARTBEAT_INTERVAL  30000       // 30秒

using namespace kugou::base::facility;
using namespace kugou::base::net;
using namespace kugou::httpproxy::liveencoder::bean;
using namespace kugou::httpproxy::liveencoder::encoder;

namespace kugou
{
namespace httpproxy
{
namespace liveencoder
{
namespace encoder
{

Heartbeat::Heartbeat(EncoderLibrary& el)
    : Runnable()
    , m_isStop(false)
    , m_pSysHealth(SysHealth::Instance())
    , m_encoderLibrary(el)
{
}

Heartbeat::~Heartbeat()
{
}

void Heartbeat::setScheduleServer(const std::string& ip, const std::string& port)
{
    m_scheduleServerIP = ip;
    m_scheduleServerPort = port;
}

void Heartbeat::setEncoderLocal(const std::string& ip, const std::string& port)
{
    m_encoderLocalIP = ip;
    m_encoderLocalPort = port;
}

void Heartbeat::run()
{
    unsigned int sleepTime = HEARTBEAT_INTERVAL;
    while(!m_isStop)
    {
        int usage = m_pSysHealth->cpuUsage();
        EncoderHeartbeatRequest request;
        std::string requestJson;
        std::string responseJson;    
        //EncoderHeartbeatResponse response;
        HTTPClient client(m_scheduleServerIP, m_scheduleServerPort, ENCODERHEARTBEAT_API);

        //设置参数
        request.Encoder_ID = (m_encoderLocalIP + (":") + m_encoderLocalPort);
        request.Usage_CPU = usage;
        m_encoderLibrary.getStreamIDs(request.StreamIDList);

        //发送命令
        requestJson = request.toJson();
        if(client.HTTPGet(requestJson, responseJson))// && response.Result.get() != NULL && (*response.Result) == "YES")
        {
            //发送心跳成功
            LOG_D("Send heart beat successful.");
            
            // 发送成功，走正常休眠时间。
            sleepTime = HEARTBEAT_INTERVAL;
        }
        else
        {
            //发送心跳失败
            LOG_D("Send heart beat failed.");

            // 发送失败，要加快再一次发送
            sleepTime = HEARTBEAT_INTERVAL / 4;
        }
        
        //Sleep
        Poco::Thread::sleep(sleepTime);
    }
}

}
}
}
}
