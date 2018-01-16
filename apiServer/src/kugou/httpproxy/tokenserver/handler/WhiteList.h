#ifndef WHITELIST_BEAN_TOKENSERVER_HTTPPROXY_KUGOU
#define WHITELIST_BEAN_TOKENSERVER_HTTPPROXY_KUGOU

#include <string>

#include <kugou/base/net/Handler.h>

namespace kugou
{
namespace httpproxy
{
namespace tokenserver
{
namespace handler
{

static const std::string sg_whiteListHttpUrl = "/api.php?api=token&action=getAllowIpList&returnType=1";

class WhiteList : public kugou::base::net::Handler
{
public:
    WhiteList(void);
	virtual ~WhiteList();
	
	std::string name()
	{
		return sg_whiteListHttpUrl;
	}
	
    WhiteList* clone(void);

protected:	
	virtual bool impl_handle(std::string &strResp);
	virtual bool impl_check();
private:
	bool check(std::string& strResponse);
	bool parser(const std::string& strReqContent, std::string& strResponse);
	bool handle(const std::string& strRmtAddr, std::string& strResp);
};

}
}
}
}

#endif
