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
    // 1:����(publish) 2:����(play)
    if(this->m_request.UsageType < 1 || this->m_request.UsageType > 2)
    {
        this->m_response.Result = 2;
        return false;
    }

    // 0:�ο� 1:��վ�û� 2:�ֻ��û�
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
    // ��ʼ����ֵ
    this->m_response.Result = 1;
    std::string UserAddr = m_pearAddress;

    // �����IP����������PHP��̨����
    if(!this->m_request.UserAddr.empty())
    {
        UserAddr = this->m_request.UserAddr;
    }
    
    // ����ǲ��Եİ���������ֱ�ӷ���domain����
	// �������relay �� allow
  	if(!StrategyManager::instance().isInWhiteList(this->m_request.StarID))
    {
        // ��������������relay�б���ģ���relay
		if(StrategyManager::instance().isRelay(this->m_request.StarID))
        {
            // ͨ���ض�������һ̨���������������
            HTTPClient client(StrategyManager::instance().getRelayIP(), \
                StrategyManager::instance().getRelayPort(), GETADDRESSLIST_API);
            std::string requestStr = this->m_request.toJson();
            std::string responseStr;
            if(client.HTTPGet(requestStr, responseStr))
            {
                //���ͳɹ�
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
                // �ض���ʧ��
                LOG_W("Relay to other access server failed.");
            }
        }

        // ��������������allow�б���ģ����ñ������õķ�����
		if(StrategyManager::instance().isAllow(this->m_request.StarID))
        {
            // �Ȳ�����IP�����Ľ����
            AccessPoint ap = AccessPointManager::instance().getAccessPoint(UserAddr); 

            if(!ap.area.empty() || !ap.carrier.empty())
            {
                // ���ҵ����������Ӧ��Щ��������(һ����������PHPֻ��һ��)��ÿ������ȡһ��������
                StreamServerManager::instance().getAddressList(ap.groupId1, this->m_response.AddressList);

                StreamServerManager::instance().getAddressList(ap.groupId2, this->m_response.AddressList);

                // ��������鶼�Ҳ���������ʧ��
                if(this->m_response.AddressList.size() == 0)
                {
                    LOG_W("No stream server avaiable in group1 and group2, usr ip [" << UserAddr << "].");
                }
                else
                {
                    // ���سɹ�
                    this->m_response.Result = 0;
                    return true;
                }
            }
            else
            {
                // �Ҳ���
                LOG_W("Can not find Access Point of ip address [" << UserAddr << "].");
            }
        }
    }

    // �������ò����з��䣬��ֱ������domain
    // ���������������һ��������ǹ��ڣ���������
    if(this->m_request.UsageType == 1)
    {
        // ������������
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
        // ���ڹ��ڹۿ�����������
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

