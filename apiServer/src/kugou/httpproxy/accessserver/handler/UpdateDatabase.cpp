#include <kugou/httpproxy/accessserver/handler/UpdateDatabase.h>
#include <kugou/httpproxy/accessserver/system/AccessServerConfig.h>
#include <kugou/httpproxy/accessserver/core/StreamServerManager.h>
#include <kugou/httpproxy/accessserver/core/AccessPointManager.h>
#include <kugou/httpproxy/accessserver/core/StrategyManager.h>
#include <kugou/base/facility/log.h>

using namespace kugou::httpproxy::accessserver::bean;
using namespace kugou::httpproxy::accessserver::system;
using namespace kugou::httpproxy::accessserver::core;

namespace kugou
{
namespace httpproxy
{
namespace accessserver
{
namespace handler
{

bool UpdateDatabase::impl_check()
{
	return true;
}

UpdateDatabase* UpdateDatabase::clone(void)
{
    return new UpdateDatabase(*this);
}

void UpdateDatabase::getPeerAddress(const std::string& peerAddress)
{
    m_pearAddress = peerAddress;
}

bool UpdateDatabase::impl_handle()
{
    // 只有白名单中的IP才能调用这个接口
    if(AccessServerConfig::instance().m_managementIP.compare(m_pearAddress))
    {
        LOG_W("IP Address [" << m_pearAddress << "] is not security to update database." );
        this->m_response.Result = -1;
        return false;
    }

    // 加载策略
    if(!StrategyManager::instance().updateStrategy())
    {
        // 加载失败
        LOG_R_E("Load strategy failed.");
        return -1;
    }

    // 同步IP库
    //if(!AccessPointManager::instance().loadGeoIP(AccessServerConfig::instance().m_geoipPath))
    if(!AccessPointManager::instance().loadIP(AccessServerConfig::instance().m_geoipPath, AccessServerConfig::instance().m_specialIPPath))
    {
        LOG_W("sync ip library failed." );
        this->m_response.Result = -2;
        return false;
    }

    // 同步服务器组和服务器
    if(!StreamServerManager::instance().flashDatabase())
    {
        LOG_W("sync server group failed." );
        this->m_response.Result = -3;
        return false;
    }

    this->m_response.Result = 0;

    return true;
}

}
}
}
}

