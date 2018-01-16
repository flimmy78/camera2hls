#include <iostream>

#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/patternlayout.h>
#include <log4cxx/fileappender.h>
#include <log4cxx/dailyrollingfileappender.h>

#include <kugou/base/net/HTTPServer.h>
#include <kugou/base/facility/log.h>
#include <kugou/httpproxy/tokenserver/handler/CreateToken.h>
#include <kugou/httpproxy/tokenserver/handler/CheckToken.h>
#include <kugou/httpproxy/tokenserver/handler/WhiteList.h>
#include <kugou/httpproxy/tokenserver/SystemConfig.h>

#include <kugou/httpproxy/tokenserver/handler/FIFOServerHandle.h>
#include <Poco/ThreadPool.h>

#ifdef _WIN32
#include <windows.h>
#define CONTRL_C_HANDLE() signal(3, _exit)
#endif

#ifdef _MSC_VER
std::string LogCFG_File = "config\\log.property";
//std::string SysConfigure_File = "config\\sys.cfg";
std::wstring LogAppender = L"A3";
#elif defined __GNUC__
std::string LogCFG_File = "config/log.property";
//std::string SysConfigure_File = "config/sys.cfg";
std::string LogAppender = "A3";
#endif

using namespace std;
using namespace kugou::httpproxy::tokenserver;
using namespace kugou::httpproxy::tokenserver::handler;

int main(int argc, char *argv[])
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
	log4cxx::PropertyConfigurator::configure(LogCFG_File);
    log4cxx::DailyRollingFileAppenderPtr t_appender(log4cxx::Logger::getRootLogger()->getAppender(LogAppender));
        
    LOG_R_I("Build date is [" << __DATE__ << " - " << __TIME__ << "].");    

    try
    {
        kugou::base::net::HTTPServer httpd(1);
        httpd.registerHandler(new CreateToken());
        httpd.registerHandler(new CheckToken());

		WhiteList hWhiteList;
		httpd.registerHandler(&hWhiteList);

		FIFOServerHandle hFifoServer(SystemConfig::instance().getFIFOFileName().c_str());
		Poco::Thread fifoThread;
        fifoThread.start(hFifoServer);
		
        std::string listenHTTPIP = SystemConfig::instance().m_HTTPServerIP;
        int listenHTTPPort = SystemConfig::instance().m_HTTPPort;
        LOG_R_I("Start up HTTP server[" << listenHTTPIP << ":" << listenHTTPPort << "].");

        httpd.serv(listenHTTPIP, listenHTTPPort);

		hFifoServer.stop();
		fifoThread.join();
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

