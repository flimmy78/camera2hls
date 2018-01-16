#include <fstream>
#include <kugou/base/net/HTTPClient.h>
#include <kugou/httpproxy/liveencoder/encoder/EncoderWorker.h>
#include <boost/algorithm/string.hpp>
#include <Poco/Process.h>
#include <Poco/PipeStream.h>
#include <Poco/StreamCopier.h>
#include <Poco/Thread.h>
#include <kugou/base/facility/log.h>
#include <kugou/httpproxy/liveencoder/bean/AddEncodeChannelRequest.h>
#include <kugou/httpproxy/liveencoder/bean/AddEncodeChannelResponse.h>
#include <kugou/httpproxy/liveencoder/SystemConfig.h>
#include <kugou/base/facility/SysHealth.h>

using namespace kugou::base::facility;
using namespace kugou::base::net;
using namespace kugou::httpproxy::liveencoder::bean;

#define  READ_BUFFER_SIZE 1024
#define RETRY_TIME 12       //重试两分钟

namespace kugou
{
namespace httpproxy
{
namespace liveencoder
{
namespace encoder
{

EncoderWorker::EncoderWorker(std::string id, std::string& src, std::string& dst)
    : Runnable()
    , m_encodeID(id)
    , m_retryTime(1)
    , m_srcStreamServer(src)
    , m_dstStreamServer(dst)
    , m_isStop(false)
{
}

bool EncoderWorker::isStop(void)
{
    return m_isStop;
}

void EncoderWorker::run()
{
    while(m_retryTime < RETRY_TIME)
    {
        LOG_I("Start encode stream [" << m_encodeID << "].");
        std::vector<std::string> args;

        args.push_back("-analyzeduration");
        args.push_back("2147483647");
        args.push_back("-probesize");
        args.push_back("2147483647");

#ifdef _WIN32_WINDOWS
        //args.push_back("-i \"rtmp://cli.live.fanxing.com/live/str" + m_encodeID + " live=1\" "); 
        args.push_back("-i \"rtmp://" + m_srcStreamServer + "/live/" + m_encodeID + " live=1\" "); 
#else
        args.push_back("-i");
        args.push_back("rtmp://" + m_srcStreamServer + "/live/" + m_encodeID + " live=1");
#endif
        args.push_back("-profile:v");
        args.push_back("baseline");
        args.push_back("-level");
        args.push_back("30");
        args.push_back("-g");
        args.push_back("25");
        args.push_back("-r");
        args.push_back("12");
        args.push_back("-b:v");
        args.push_back("150k");
        //args.push_back("-s");
        //args.push_back("320x240");
        args.push_back("-vcodec");
        args.push_back("libx264");
        args.push_back("-acodec");             
#ifdef _WIN32_WINDOWS
        args.push_back("libvo_aacenc");   
#else
        args.push_back("aac");
#endif
        args.push_back("-ar");
        args.push_back("44100");
        args.push_back("-ab");
        args.push_back("48k");
        args.push_back("-threads");
        args.push_back("1");
        args.push_back("-strict");
        args.push_back("-2");
        args.push_back("-f");
        args.push_back("flv");
        //args.push_back(m_encodeDestinationUrl);// + "\" -loglevel ");
        //args.push_back("rtmp://" + m_dstStreamServer + "/live/mobile" + m_encodeID);
        args.push_back("rtmp://" + m_dstStreamServer + "/live/low_" + m_encodeID);

        //args.push_back("-loglevel");
        //args.push_back("quiet");
		
        std::string printInfo;
        for(int i = 0; i < args.size(); i++)
        {
            printInfo.append(args[i]);
            printInfo.append(" ");
        }
		
		LOG_I("./ffmpeg/ffmpeg " << printInfo );

        Poco::Pipe outPipe;
		unsigned int lunchPID;
        {
            boost::mutex::scoped_lock scopedLock(m_mutexProcessHandler);
#ifdef _WIN32_WINDOWS
            m_processHandler.reset(new Poco::ProcessHandle(Poco::Process::launch("ffmpeg", args, 0, 0, &outPipe)));
#else
            m_processHandler.reset(new Poco::ProcessHandle(Poco::Process::launch("./ffmpeg/ffmpeg", args, 0, 0, &outPipe)));
#endif
			lunchPID = m_processHandler->id();
        }
        Poco::PipeInputStream istr(outPipe);

        {
            char buf[READ_BUFFER_SIZE + 1];	    
            memset(buf,'\0',READ_BUFFER_SIZE);
            std::streamsize n;
            int step = 0;
	        do
	        {
		        if (istr)
		        {
                    memcpy(buf, buf + READ_BUFFER_SIZE - 5, 5);
                    istr.read(buf + 5, READ_BUFFER_SIZE - 5);
                    n = istr.gcount();
                    if(step == 0 && boost::algorithm::find_first(buf, "Output"))
                    {
                        step = 1;
                    }
                    if(step == 1 && boost::algorithm::find_first(buf, "Video: h264"))
                    {
                        LOG_I("Start encode stream [" << m_encodeID << "] successful.");

                        AddChannelRequest request;
                        std::string requestJson;
                        std::string responseJson;                        
                        //AddChannelResponse response;
                        std::string scheduleServerIP = SystemConfig::Singleton.get()->m_scheduleServerIP;
                        std::string scheduleServerPort = boost::lexical_cast <std::string> (SystemConfig::Singleton.get()->m_scheduledServerPort);
                        HTTPClient client(scheduleServerIP, scheduleServerPort, ADDCHANNELSUCCESSREQUEST_API);
 
                        //设置参数
                        request.Source_ID = m_encodeID;
       
                         //发送命令
                        requestJson = request.toJson();
                        if(client.HTTPGet(requestJson, responseJson))// && response.Result.get() != NULL && (*response.Result) == "YES")
                        {
                            m_retryTime = 1;
                            //发送成功
                            LOG_D("Report to SEC successful.");
                        }
                        else
                        {
                            //发送心跳失败
                            LOG_E("Report to SEC failed.");
                        }
                        break;
                    }
		        }
		        else n = 0;
	        }
            while(n > 0);
        }

        outPipe.close();
        istr.close();

#ifdef _WIN32_WINDOWS
        //Sleep 10 second,注意要先sleep 再wait.要不然stop这个线程时，就要sleep10 second了
        Poco::Thread::sleep(10000);
#else
		// 如果线程退出，PID目录没有了，getPidMemory返回-1
		long long pidMomory = 0;
		do
		{
			pidMomory = SysHealth::Instance()->getPidMemory(lunchPID);
			// 内存占用大于200M退出
			if(pidMomory > 200*1024)
			{
				Poco::Process::kill(lunchPID);
			}
			Poco::Thread::sleep(5000);
		}
		while(pidMomory > 1024);
#endif
		Poco::Process::wait(*m_processHandler);
        
        LOG_I("Stop encode stream [" << m_encodeID << "].");

        m_retryTime++;
    }

    //线程退出
    m_isStop = true;
}

void EncoderWorker::stop(void)
{
    //boost::mutex::scoped_lock scopedLock(m_mutexProcessHandler);
    //LOG_I("Ready to stop encode stream [" << m_encodeID << "]. its PID[" << m_processHandler->id()\
    //    << "]. its parent PID [" << Poco::Process::id() << "].");

    m_retryTime = RETRY_TIME;
}

}
}
}
}
