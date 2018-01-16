#include <kugou/base/facility/log.h>
#include <kugou/httpproxy/accessserver/handler/JustReportTestResult.h>

#include <boost/lexical_cast.hpp>

using namespace kugou::httpproxy::accessserver::bean;

namespace kugou
{
namespace httpproxy
{
namespace accessserver
{
namespace handler
{

bool JustReportTestResult::impl_check()
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

void JustReportTestResult::getPeerAddress(const std::string& peerAddress)
{
    m_pearAddress = peerAddress;
}

JustReportTestResult* JustReportTestResult::clone(void)
{
    return new JustReportTestResult(*this);
}

bool JustReportTestResult::impl_handle()
{
    LOG_D("just receive test result UsageType [" << m_request.UsageType << \
        "] UserType [" << m_request.UserType << \
        "] UserID [" << m_request.UserID << \
        "] RoomID [" << m_request.RoomID<< \
        "] GroupID [" << m_request.ID << \
        "] Speed [" << m_request.Speed << \
        "] PeerIP [" << m_pearAddress << "]");

	this->m_response.Result = 0;

    return true;
}

}
}
}
}
