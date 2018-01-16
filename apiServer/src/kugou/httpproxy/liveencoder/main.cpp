#include <string>
#ifdef __GNUC__
#include <sys/resource.h>
#endif

#include <kugou/base/net/HTTPClient.h>
#include <kugou/httpproxy/liveencoder/handler/AddEncodeChannel.h>
#include <kugou/httpproxy/liveencoder/handler/DelEncodeChannel.h>
#include <kugou/httpproxy/liveencoder/handler/RegisterEncoder.h>
#include <kugou/httpproxy/liveencoder/handler/EncoderHeartbeat.h>
#include <kugou/httpproxy/liveencoder/handler/AddEncodeChannelSuccess.h>
#include <kugou/httpproxy/liveencoder/encoder/EncoderLibrary.h>
#include <kugou/httpproxy/liveencoder/SystemConfig.h>
#include <kugou/httpproxy/liveencoder/scheduleencoder/ScheduleEncoderLibrary.h>

#include <kugou/base/facility/log.h>
#include <kugou/httpproxy/liveencoder/encoder/Heartbeat.h>
#include <kugou/httpproxy/liveencoder/bean/AddEncodeChannelRequest.h>
#include <kugou/httpproxy/liveencoder/bean/AddEncodeChannelResponse.h>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/lexical_cast.hpp>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/patternlayout.h>
#include <log4cxx/fileappender.h>
#include <log4cxx/dailyrollingfileappender.h>
#include <Poco/ThreadPool.h>
#include <kugou/base/net/HTTPServer.h>


#ifdef _WIN32
#include <windows.h>
#define CONTRL_C_HANDLE() signal(3, _exit)
#endif

#ifdef _MSC_VER
std::string LogCFG_File = "config\\log.property";
std::string SysConfigure_File = "config\\sys.cfg";
std::wstring LogAppender = L"A3";
std::string Archive_File = "\\archive";
#elif defined __GNUC__
std::string LogCFG_File = "config/log.property";
std::string SysConfigure_File = "config/sys.cfg";
std::string LogAppender = "A3";
std::string Archive_File = "/archive";
#endif

using namespace kugou::httpproxy::liveencoder;
using namespace kugou::httpproxy::liveencoder::handler;
using namespace kugou::httpproxy::liveencoder::bean;
using namespace kugou::httpproxy::liveencoder::encoder;
using namespace kugou::httpproxy::liveencoder::scheduleencoder;
using namespace kugou::base::facility;

using namespace boost;

int main(int, char*[])
{
#ifdef _WIN32
	WSADATA WSAData;
	WSAStartup(0x101, &WSAData);
    CONTRL_C_HANDLE();
#endif

#ifdef __linux__
    //daemon(1,0) will not changes the current working directory to the root ("/"). 
    //daemon(0,0) will redirect standard input, standard output and standard error to /dev/null. 
    if(daemon(1,0) == -1)
    {
         //return -1; //will not exit
    }
#endif

    try
    {
        log4cxx::PropertyConfigurator::configure(LogCFG_File);
        log4cxx::DailyRollingFileAppenderPtr t_appender(log4cxx::Logger::getRootLogger()->getAppender(LogAppender));
        
        LOG_R_I("Build date is [" << __DATE__ << " - " << __TIME__ << "].");    

#ifdef __GNUC__
        struct rlimit core_limits;
        // core file size (blocks) [#getconf PAGESIZE] alwlay 4K, we set to 500M
        core_limits.rlim_cur = core_limits.rlim_max = 0;//128000;//RLIM_INFINITY;
        if(setrlimit(RLIMIT_CORE, &core_limits) < 0)
        {
            LOG_R_F("Cannot modify core file size limit: Operation not permitted.");
            return -1;
        }
#endif

      
        SystemConfig* sc = SystemConfig::Singleton.get();//SystemConfig::create(SysConfigure_File);
        
        if(sc->m_isScheduleServer)
        {
            LOG_R_I("Lunch cheduled server ...");

            // 建立调度服务器HTTP服务器
            //LOG_R_I("Ready to start up schedule server interface [" << sc->m_scheduleServerIP << ":" << sc->m_scheduledServerPort << "].");
            //HTTPServer *hs = new HTTPServer(sc->m_scheduleServerIP, lexical_cast <std::string> (sc->m_scheduledServerPort),"D:\\");
            kugou::base::net::HTTPServer hs(1);
            //LOG_R_I("Start up schedule server interface[" << sc->m_scheduleServerIP << ":" << sc->m_scheduledServerPort << "] successful.");

            // 注册调度服务器接口
            ScheduleEncoderLibrary* sel = new ScheduleEncoderLibrary();
            hs.registerHandler(new RegisterEncoder(*sel));
            hs.registerHandler(new EncoderHeartbeat(*sel));
            hs.registerHandler(new AddEncodeChannel(*sel));
            hs.registerHandler(new DelEncodeChannel(*sel));
            hs.registerHandler(new AddEncodeChannelSuccess(*sel));     

            // 读出所有启动前保存的EC信息，并恢复EC
            std::string currentPath = boost::filesystem::initial_path<boost::filesystem::path>().string();
            boost::filesystem::path archivePath(currentPath.append(Archive_File));
            if(boost::filesystem::exists(archivePath))
            {
                boost::filesystem::directory_iterator itBegin(archivePath);
                boost::filesystem::directory_iterator itEnd;

                for(;itBegin != itEnd; itBegin++)
                {
                    try
                    {
                        if (!boost::filesystem::is_directory(*itBegin))
                        {   
                            EncoderHeartbeatRequest req;
                            if(req.fromJsonFile(itBegin->path().string()))
                            {
                                std::vector<std::string> idVector;
                                sel->addEncoder(req.Encoder_ID, idVector);
                                std::vector<std::string>::iterator itBegin = req.StreamIDList.begin();
                                for(;itBegin != req.StreamIDList.end(); itBegin++)
                                {
                                    if(!sel->addEncodeStreamID(*itBegin, req.Encoder_ID))
                                    {
                                        break;
                                    }
                                    Poco::Thread::sleep(1000);
                                }
                            }
                            else
                            {
                                LOG_R_E("Parse archive file : " <<  itBegin->path().string() << " to Json Error.");
                            }
                        }
                    } 
                    catch (const std::exception & ex)
                    {
                        LOG_R_E("Load archive file Error : " << ex.what());
                        continue;
                    }
                }
            }
            else
            {
                LOG_R_E("Can not find archive path : " << archivePath.string());
            }

            std::string listenHTTPIP = sc->m_scheduleServerIP;
            int listenHTTPPort = sc->m_scheduledServerPort;     
            LOG_R_I("Start up HTTP server[" << listenHTTPIP << ":" << listenHTTPPort << "].");
            hs.serv(listenHTTPIP, listenHTTPPort);
            //hs->run();
        }
        else
        {
            LOG_R_I("Lunch encoder ...");
            LOG_R_I("Set source Stream Server [" << sc->m_srcStreamServer << "].");
            LOG_R_I("Set destination Stream Server [" << sc->m_dstStreamServer << "].");

            // 建立编码服务器HTTP服务器
            //LOG_R_I("Ready to start up encoder server[" << sc->m_encoderLocalIP << ":" << sc->m_encoderLocalPort << "].");
            //HTTPServer *hs = new HTTPServer(sc->m_encoderLocalIP, lexical_cast <std::string> (sc->m_encoderLocalPort),"D:\\");
            kugou::base::net::HTTPServer hs(1);
            //LOG_R_I("Start up encoder server[" << sc->m_encoderLocalIP << ":" << sc->m_encoderLocalPort << "] successful.");

            // 注册编码器接口
            Poco::ThreadPool *tp = new Poco::ThreadPool(2,32);
            EncoderLibrary  *el = new EncoderLibrary(*tp, sc->m_srcStreamServer, sc->m_dstStreamServer);

            hs.registerHandler(new AddEncodeChannel(*el));
            hs.registerHandler(new DelEncodeChannel(*el));

            //连接并注册到调度服务器
            RegisterEncoderRequest request;
            std::string requestJson;
            std::string responseJson;  
            RegisterEncoderResponse response;
            kugou::base::net::HTTPClient client(sc->m_scheduleServerIP, boost::lexical_cast <std::string> (sc->m_scheduledServerPort), REGISTERENCODER_API);
            request.Encoder_ID = sc->m_encoderLocalIP + (":") + boost::lexical_cast <std::string> (sc->m_encoderLocalPort);
            
            requestJson = request.toJson();
            if(client.HTTPGet(requestJson, responseJson))// && response.Result.get() != NULL && (*response.Result) == "YES")
            {
                //注册成功
                LOG_R_I("Register encoder [" << (request.Encoder_ID) << "] successful.");

                response.fromJson(responseJson);
                std::string streamIDList;
                std::vector<std::string>::iterator it = response.StreamIDList.begin();
                while(it != response.StreamIDList.end())
                {
                    el->addEncodeStreamID(*it);
                    Poco::Thread::sleep(1000);
                    ++it;
                }
            }
            else
            {
                //注册失败
                LOG_R_E("Register encoder [" << (request.Encoder_ID) << "] failed.");
                exit(1);
            }

            // 启动心跳线程
            Poco::Thread heartbeatThread;
            Heartbeat runnableHeartbeat(*el);
            runnableHeartbeat.setScheduleServer(sc->m_scheduleServerIP,boost::lexical_cast <std::string> (sc->m_scheduledServerPort));
            runnableHeartbeat.setEncoderLocal(sc->m_encoderLocalIP, boost::lexical_cast <std::string> (sc->m_encoderLocalPort));
            heartbeatThread.start(runnableHeartbeat);

            std::string listenHTTPIP = sc->m_encoderLocalIP;
            int listenHTTPPort = sc->m_encoderLocalPort;     
            LOG_R_I("Start up HTTP server[" << listenHTTPIP << ":" << listenHTTPPort << "].");
            hs.serv(listenHTTPIP, listenHTTPPort);
            //hs->run();
        }
    }
    catch(std::exception& e)
    {
        LOG_R_F("Exception occured : " << e.what());
    }
    catch(...)
    {
        LOG_R_F("Unpredictable exception occured.");        
    }

    return 0;
}
