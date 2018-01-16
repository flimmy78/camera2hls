#include <kugou/base/facility/log.h>
#include <kugou/base/net/HTTPClient.h>
#include <kugou/httpproxy/accessserver/handler/GetTestNodeList.h>
#include <kugou/httpproxy/accessserver/core/StreamServerManager.h>
#include <kugou/httpproxy/accessserver/core/AccessPointManager.h>
#include <kugou/httpproxy/accessserver/core/StrategyManager.h>

using namespace kugou::base::net;
using namespace kugou::httpproxy::accessserver::bean;
using namespace kugou::httpproxy::accessserver::core;


namespace kugou
{
namespace httpproxy
{
namespace accessserver
{
namespace handler
{

bool GetTestNodeList::impl_check()
{
    // 1:发布(publish) 2:播放(play)
    if(this->m_request.UsageType < 1 || this->m_request.UsageType > 2)
    {
        this->m_response.Result = 2;
        return false;
    }

    // 0:游客 1:网站用户 2:手机用户
    if(this->m_request.UserType < 0 || this->m_request.UserType > 2)
    {
        this->m_response.Result = 3;
        return false;
    }
	return true;
}

GetTestNodeList* GetTestNodeList::clone(void)
{
    return new GetTestNodeList(*this);
}

void GetTestNodeList::getPeerAddress(const std::string& peerAddress)
{
    m_pearAddress = peerAddress;
}

bool GetTestNodeList::impl_handle()
{
	// 如果是策略的白名单，就直接返回本系统的测试结点
	// 否则进入relay 来获取
	std::string starID = (this->m_request.UsageType == 1) ? this->m_request.UserID : this->m_request.StarID;
  	if(!StrategyManager::instance().isInWhiteList(starID))
    {
		// 如果这个房间是在relay列表里的，就relay
		if(StrategyManager::instance().isRelay(starID))
        {
            // 通过重定向到另外一台接入服务器收请求
            HTTPClient client(StrategyManager::instance().getRelayIP(), \
                StrategyManager::instance().getRelayPort(), GETTESTNODELIST_API);
            std::string requestStr = this->m_request.toJson();
            std::string responseStr;
            if(client.HTTPGet(requestStr, responseStr))
            {
                //发送成功
                GetTestNodeListResponse response;
                if(response.fromJson(responseStr))
                {
                    LOG_I("Relay to other access server and get it successful.");
                    this->m_response = response;
                    return true;
                }
            }
            else
            {
                // 重定向失败
                LOG_W("Relay to other access server failed.");
            }
		}
	}

    // 先查出这个IP所属的接入点
    AccessPoint ap = AccessPointManager::instance().getAccessPointNoSpecial(m_pearAddress); 

    if(!ap.area.empty() || !ap.carrier.empty())
    {
        this->m_response.UserCarrier = ap.carrier;
        this->m_response.UserArea = ap.area;        
    }
    else
    {
        // 找不到接入点，返回地区和运营商为空
        this->m_response.UserCarrier = " ";
        this->m_response.UserArea = " "; 

        LOG_W("Can not find Access Point of ip address [" << m_pearAddress << "].");
    }

    // 返回对方的出口IP地址
    this->m_response.UserAddr = m_pearAddress;

    if(StreamServerManager::instance().getTestServerList(this->m_response.NodeList))
    {
        this->m_response.Result = 0;
    }
    else
    {
        // 找不到测试IDC点
        LOG_W("Can not find IDC test node.");
        this->m_response.Result = 1;
    }

    LOG_D("user get test node UserIP[" << this->m_response.UserAddr << \
        "] UserCarrier [" << this->m_response.UserCarrier << \
        "] UserArea [" << this->m_response.UserArea << "]");

    return true;
}

}
}
}
}

