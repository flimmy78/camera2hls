#include <kugou/base/facility/log.h>
#include <kugou/base/net/HTTPClient.h>
#include <kugou/httpproxy/accessserver/handler/GetAddressList.h>
#include <kugou/httpproxy/accessserver/core/AccessPointManager.h>
#include <kugou/httpproxy/accessserver/core/StreamServerManager.h>
#include <kugou/httpproxy/accessserver/core/StrategyManager.h>
#include <kugou/httpproxy/accessserver/system/AccessServerConfig.h>

using namespace kugou::base::net;
using namespace kugou::httpproxy::accessserver::bean;
using namespace kugou::httpproxy::accessserver::core;
using namespace kugou::httpproxy::accessserver::system;

namespace kugou
{
namespace httpproxy
{
namespace accessserver
{
namespace handler
{

bool GetAddressList::impl_check()
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

GetAddressList* GetAddressList::clone(void)
{
    return new GetAddressList(*this);
}

void GetAddressList::getPeerAddress(const std::string& peerAddress)
{
    m_pearAddress = peerAddress;
}

bool GetAddressList::impl_handle()
{
    // 初始返回值
    this->m_response.Result = 1;
    std::string UserAddr = m_pearAddress;

    // 如果带IP过来，就是PHP后台调用
    if(!this->m_request.UserAddr.empty())
    {
        UserAddr = this->m_request.UserAddr;
    }
    
    // 如果是策略的白名单，就直接返回domain域名
	// 否则进入relay 或 allow
  	if(!StrategyManager::instance().isInWhiteList(this->m_request.StarID))
    {
        // 如果这个房间是在relay列表里的，就relay
		if(StrategyManager::instance().isRelay(this->m_request.StarID))
        {
            // 通过重定向到另外一台接入服务器收请求
            HTTPClient client(StrategyManager::instance().getRelayIP(), \
                StrategyManager::instance().getRelayPort(), GETADDRESSLIST_API);
            std::string requestStr = this->m_request.toJson();
            std::string responseStr;
            if(client.HTTPGet(requestStr, responseStr))
            {
                //发送成功
                GetAddressListResponse response;
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

        // 如果这个房间是在allow列表里的，就用本地配置的服务器
		if(StrategyManager::instance().isAllow(this->m_request.StarID))
        {
            // 先查出这个IP所属的接入点
            AccessPoint ap = AccessPointManager::instance().getAccessPoint(UserAddr); 

            if(!ap.area.empty() || !ap.carrier.empty())
            {
                // 再找到这个接入点对应那些服务器组(一般两个，给PHP只给一个)，每个组里取一个流服务
                StreamServerManager::instance().getAddressList(ap.groupId1, this->m_response.AddressList);

                StreamServerManager::instance().getAddressList(ap.groupId2, this->m_response.AddressList);

                // 如果两个组都找不到，返回失败
                if(this->m_response.AddressList.size() == 0)
                {
                    LOG_W("No stream server avaiable in group1 and group2, usr ip [" << UserAddr << "].");
                }
                else
                {
                    // 返回成功
                    this->m_response.Result = 0;
                    return true;
                }
            }
            else
            {
                // 找不到
                LOG_W("Can not find Access Point of ip address [" << UserAddr << "].");
            }
        }
    }

    // 不在配置策略中房间，就直播返回domain
    // 如果是主播，返回一个，如果是观众，返回两个
    if(this->m_request.UsageType == 1)
    {
        // 属于主播推流
        AddressListObject obj;
        obj.Port = "1935";            
        obj.Addr = "src.live.fanxing.com";
        this->m_response.AddressList.push_back(obj);

        AddressListObject obj2;
        obj2.Port = "80";
        obj2.Addr = "src.live.fanxing.com";
        this->m_response.AddressList.push_back(obj2);
    }
    else
    {
        // 属于观众观看，返回两个
        AddressListObject obj;
        obj.Port = "1935";      
        obj.Addr = "cli.live.fanxing.com";
        this->m_response.AddressList.push_back(obj);

        AddressListObject obj2;
        obj2.Port = "80";      
        obj2.Addr = "cli.live.fanxing.com";
        this->m_response.AddressList.push_back(obj2);
    }
        
    this->m_response.Result = 0;

    return true;
}

}
}
}
}

