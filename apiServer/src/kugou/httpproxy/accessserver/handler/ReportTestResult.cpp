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

	// ����ǲ��Եİ���������ֱ�ӷ��ر�ϵͳ����������
	// �������relay ����ȡ
	std::string starID = (this->m_request.UsageType == 1) ? this->m_request.UserID : this->m_request.StarID;
	if(!StrategyManager::instance().isInWhiteList(starID))
    {
		// ��������������relay�б���ģ���relay
		if(StrategyManager::instance().isRelay(starID))
        {
            // ͨ���ض�������һ̨���������������
            HTTPClient client(StrategyManager::instance().getRelayIP(), \
                StrategyManager::instance().getRelayPort(), RESPORTTESTRESULT_API);
            std::string requestStr = this->m_request.toJson();
            std::string responseStr;
            if(client.HTTPGet(requestStr, responseStr))
            {
                //���ͳɹ�
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
                // �ض���ʧ��
                LOG_W("ReportTestResult: Relay to other access server failed.");
            }
		}
	}

    // �鿴�ϱ����ٽڵ�(IDC����)����һ�����ʷ���������
    if(StreamServerManager::instance().getAddressList(boost::lexical_cast<int> (this->m_request.ID), this->m_response.AddressList))
    {
        // �ҵ�IDC���������ɹ��ҵ���Ӧ������
        this->m_response.Result = 0;
    }
    else
    {
        // û���ҵ�IDC����������ʧ��
        this->m_response.Result = 1;
        LOG_W("Can not find IDC room [" << this->m_request.ID << "], or no stream server avilable.");
    }

    return true;
}

}
}
}
}
