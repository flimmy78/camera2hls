#include <kugou/httpproxy/accessserver/handler/IPSearch.h>
#include <kugou/httpproxy/accessserver/core/AccessPointManager.h>
#include <kugou/httpproxy/accessserver/core/StreamServerManager.h>

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

bool IPSearch::impl_check()
{
    if(this->m_request.Addr.empty())
    {
        this->m_response.Result = -1;
        return false;
    }
	return true;
}

IPSearch* IPSearch::clone(void)
{
    return new IPSearch(*this);
}

void IPSearch::getPeerAddress(const std::string& peerAddress)
{
    std::string addr = peerAddress;
}

bool IPSearch::impl_handle()
{
    // http://192.168.8.145:8008/api/access/ipsearch.json?callback=ipsearch&jsonStr={"Addr":"111.39.0.3"}&

    // 先查出这个IP所属的接入点
    AccessPoint ap = AccessPointManager::instance().getAccessPoint(this->m_request.Addr);
    
    if(!ap.area.empty() || !ap.carrier.empty())
    {
        this->m_response.Result = 0;
        this->m_response.Area = ap.area;
        this->m_response.Carrier = ap.carrier;

        // 找到这个接入点对应那些服务器组(一般两个)，每个组里取一个流服务
        StreamServerManager::instance().getAddressList(ap.groupId1, this->m_response.AddressList);
        StreamServerManager::instance().getAddressList(ap.groupId2, this->m_response.AddressList);
    }

    // 如果都没有找到，返回domain域名
    if(this->m_response.AddressList.size() == 0)
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
        this->m_response.Result = 0;
    }

    return true;
}

}
}
}
}

