/**
 * Title: Interface Handler
 * Description: Handler add encode channel api interface
 * Copyright: Copyright (c) 2013 Kugou (All rights reserved)
 * Company: Kugou
 * Author: zhiguangq
 * Reviser: <Please input reviser>
 * Date: 2013-9-24
 * Version: 1.0
 * History: [1.0 2013-9-24]
 */


#ifndef CREATETOKEN_BEAN_TOKENSERVER_HTTPPROXY_KUGOU
#define CREATETOKEN_BEAN_TOKENSERVER_HTTPPROXY_KUGOU

#include <string>

#include <kugou/base/net/ImplHandler.h>
#include <kugou/httpproxy/tokenserver/bean/CreateTokenRequest.h>
#include <kugou/httpproxy/tokenserver/bean/CreateTokenResponse.h>

namespace kugou
{
namespace httpproxy
{
namespace tokenserver
{
namespace handler
{

class CreateToken : public kugou::base::net::ImplHandler<kugou::httpproxy::tokenserver::bean::CreateTokenRequest,\
                        kugou::httpproxy::tokenserver::bean::CreateTokenResponse>
{
public:
    CreateToken(void)
    {
    }
	std::string name()
	{
		return CREATETOKENREQUEST_API;
	}
    CreateToken* clone(void);
protected:
	virtual bool impl_handle();
	virtual bool impl_check();
};

}
}
}
}

#endif
