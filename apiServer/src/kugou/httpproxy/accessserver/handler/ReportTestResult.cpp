#include <kugou/base/facility/log.h>
#include <kugou/base/net/HTTPClient.h>
#include <kugou/httpproxy/accessserver/handler/ReportTestResult.h>
#include <kugou/httpproxy/accessserver/core/StreamServerManager.h>
#include <kugou/httpproxy/accessserver/core/StrategyManager.h>

#include <boost/lexical_cast.hpp>

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

bool ReportTestResult::impl_check()
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

void ReportTestResult::getPeerAddress(const std::string& peerAddress)
{
    m_pearAddress = peerAddress;
}

ReportTestResult* ReportTestResult::clone(void)
{
    return new ReportTestResult(*this);
}

bool ReportTestResult::impl_handle()
{
    LOG_D("receive test result UsageType [" << m_request.UsageType << \
        "] UserType [" << m_request.UserType << \
        "] UserID [" << m_request.UserID << \
        "] RoomID [" << m_request.RoomID<< \
        "] GroupID [" << m_request.ID << \
        "] Speed [" << m_request.Speed << \
        "] PeerIP [" << m_pearAddress << "]");

	// 如果是策略的白名单，就直接返回本系统的流服务器
	// 否则进入relay 来获取
	std::string starID = (this->m_request.UsageType == 1) ? this->m_request.UserID : this->m_request.StarID;
	if(!StrategyManager::instance().isInWhiteList(starID))
    {
		// 如果这个房间是在relay列表里的，就relay
		if(StrategyManager::instance().isRelay(starID))
        {
            // 通过重定向到另外一台接入服务器收请求
            HTTPClient client(StrategyManager::instance().getRelayIP(), \
                StrategyManager::instance().getRelayPort(), RESPORTTESTRESULT_API);
            std::string requestStr = this->m_request.toJson();
            std::string responseStr;
            if(client.HTTPGet(requestStr, responseStr))
            {
                //发送成功
                ReportTestResultResponse response;
                if(response.fromJson(responseStr))
                {
                    LOG_I("ReportTestResult: Relay to other access server and get it successful.");
                    this->m_response = response;
                    return true;
                }
            }
            else
            {
                // 重定向失败
                LOG_W("ReportTestResult: Relay to other access server failed.");
            }
		}
	}

    // 查看上报测速节点(IDC机房)，找一个合适服务器返回
    if(StreamServerManager::instance().getAddressList(boost::lexical_cast<int> (this->m_request.ID), this->m_response.AddressList))
    {
        // 找到IDC机房，并成功找到相应服务器
        this->m_response.Result = 0;
    }
    else
    {
        // 没有找到IDC机房，返回失败
        this->m_response.Result = 1;
        LOG_W("Can not find IDC room [" << this->m_request.ID << "], or no stream server avilable.");
    }

    return true;
}

}
}
}
}
