#include <iostream>

#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/patternlayout.h>
#include <log4cxx/fileappender.h>
#include <log4cxx/dailyrollingfileappender.h>

#include <kugou/base/net/HTTPServer.h>
#include <kugou/base/facility/log.h>
#include <kugou/httpproxy/accessserver/system/AccessServerConfig.h>
#include <kugou/httpproxy/accessserver/handler/GetAddressList.h>
#include <kugou/httpproxy/accessserver/handler/GetTestNodeList.h>
#include <kugou/httpproxy/accessserver/handler/ReportTestResult.h>
#include <kugou/httpproxy/accessserver/handler/JustReportTestResult.h>
#include <kugou/httpproxy/accessserver/handler/UpdateDatabase.h>
#include <kugou/httpproxy/accessserver/handler/IPSearch.h>
#include <kugou/httpproxy/accessserver/core/RadixTree.h>
#include <kugou/httpproxy/accessserver/core/AccessPointManager.h>
#include <kugou/httpproxy/accessserver/core/StreamServerManager.h>
#include <kugou/httpproxy/accessserver/core/StrategyManager.h>

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
using namespace kugou::httpproxy::accessserver::system;
using namespace kugou::httpproxy::accessserver::handler;
using namespace kugou::httpproxy::accessserver::core;

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
        // 加载策略
        if(!StrategyManager::instance().updateStrategy())
        {
            // 加载失败
            LOG_R_E("Load strategy failed.");
            return -1;
        }

        // 加载IP库
        if(!AccessPointManager::instance().loadIP(AccessServerConfig::instance().m_geoipPath, AccessServerConfig::instance().m_specialIPPath))
        {
            // 加载失败
            LOG_R_E("Load ip failed.");
            return -1;
        }

        // 加载数据库中的服务器组等信息
        if(!StreamServerManager::instance().flashDatabase())
        {
            // 加载失败
            LOG_R_E("Load database failed.");
            return -1;
        }

        if(AccessServerConfig::instance().m_CPUNumber < 1)
        {
            LOG_R_E("CPU Number is smaller than 1.");
            return -1;
        }

        LOG_R_I("Set CPU Number [" << AccessServerConfig::instance().m_CPUNumber << "].");

        kugou::base::net::HTTPServer httpd(AccessServerConfig::instance().m_CPUNumber);
        httpd.registerHandler(new GetAddressList());
        httpd.registerHandler(new GetTestNodeList());
        httpd.registerHandler(new ReportTestResult());
		httpd.registerHandler(new JustReportTestResult());
        httpd.registerHandler(new UpdateDatabase());
        httpd.registerHandler(new IPSearch());

        std::string listenHTTPIP = AccessServerConfig::instance().m_HTTPServerIP;
        int listenHTTPPort = AccessServerConfig::instance().m_HTTPPort;
        LOG_R_I("Start up HTTP server[" << listenHTTPIP << ":" << listenHTTPPort << "].");

        httpd.serv(listenHTTPIP, listenHTTPPort);
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

