#include <boost/lexical_cast.hpp>

#include <kugou/base/facility/log.h>
#include <kugou/base/net/GeneralError.h>

#include <kugou/httpproxy/tokenserver/handler/WhiteList.h>
#include <kugou/httpproxy/tokenserver/handler/WhiteListXml.h>

using namespace kugou::base::facility;

namespace kugou
{
namespace httpproxy
{
namespace tokenserver
{
namespace handler
{

WhiteList::WhiteList(void)
{
}

WhiteList::~WhiteList()
{

}

WhiteList* WhiteList::clone(void)
{
    return new WhiteList(*this);
}

bool WhiteList::handle(const std::string& strRmtAddr, std::string& strResp)
{
	if (impl_handle(strResp))
	{
		//LOG_D("WhiteList the Result of Handle:\n" << strResp);
		return true;
	}

	strResp.clear();
	
	return false;
}

bool WhiteList::parser(const std::string& strReqContent, std::string& strResponse)
{
	try
	{
#ifdef NEVER
		//re-initialization members, or else the cur-value is pre-value
		m_request = Request();

        if(!inJson.empty() && m_request.fromJson(inJson) == false)
        {
            strResponse = kugou::base::net::GeneralError("JSON_ERROR", "parse json error").toJson();
            return false;
        }
		//LOG_T(_T("convert json string to business object successful inJson:[\n")<<inJson<<_T("]"));
#endif /* NEVER */
		return true;
	}
	catch(...)
	{
		strResponse = kugou::base::net::GeneralError("PARAM_ERROR", "parse json error").toJson();
		return false;
	}
}

bool WhiteList::check(std::string& strResponse)
{
	if (!impl_check())
	{
		return false;
	}
	
	return true;
}

bool WhiteList::impl_check()
{
	return true;
}

bool WhiteList::impl_handle(std::string &strResp)
{
	if (WhiteListXml::getInstance()->getWhiteList(strResp))
	{
		//LOG_D("WhiteList the Result of Handle:\n" << strResp);
		return true;
	}

	return false;

}

}
}
}
}
