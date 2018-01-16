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
	// ����ǲ��Եİ���������ֱ�ӷ��ر�ϵͳ�Ĳ��Խ��
	// �������relay ����ȡ
	std::string starID = (this->m_request.UsageType == 1) ? this->m_request.UserID : this->m_request.StarID;
  	if(!StrategyManager::instance().isInWhiteList(starID))
    {
		// ��������������relay�б���ģ���relay
		if(StrategyManager::instance().isRelay(starID))
        {
            // ͨ���ض�������һ̨���������������
            HTTPClient client(StrategyManager::instance().getRelayIP(), \
                StrategyManager::instance().getRelayPort(), GETTESTNODELIST_API);
            std::string requestStr = this->m_request.toJson();
            std::string responseStr;
            if(client.HTTPGet(requestStr, responseStr))
            {
                //���ͳɹ�
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
                // �ض���ʧ��
                LOG_W("Relay to other access server failed.");
            }
		}
	}

    // �Ȳ�����IP�����Ľ����
    AccessPoint ap = AccessPointManager::instance().getAccessPointNoSpecial(m_pearAddress); 

    if(!ap.area.empty() || !ap.carrier.empty())
    {
        this->m_response.UserCarrier = ap.carrier;
        this->m_response.UserArea = ap.area;        
    }
    else
    {
        // �Ҳ�������㣬���ص�������Ӫ��Ϊ��
        this->m_response.UserCarrier = " ";
        this->m_response.UserArea = " "; 

        LOG_W("Can not find Access Point of ip address [" << m_pearAddress << "].");
    }

    // ���ضԷ��ĳ���IP��ַ
    this->m_response.UserAddr = m_pearAddress;

    if(StreamServerManager::instance().getTestServerList(this->m_response.NodeList))
    {
        this->m_response.Result = 0;
    }
    else
    {
        // �Ҳ�������IDC��
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

