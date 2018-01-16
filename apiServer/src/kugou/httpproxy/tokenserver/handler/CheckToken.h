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


#ifndef CHECKTOKEN_BEAN_TOKENSERVER_HTTPPROXY_KUGOU_H
#define CHECKTOKEN_BEAN_TOKENSERVER_HTTPPROXY_KUGOU_H

#include <string>

#include <kugou/base/net/ImplHandler.h>
#include <kugou/httpproxy/tokenserver/bean/CheckTokenRequest.h>
#include <kugou/httpproxy/tokenserver/bean/CheckTokenResponse.h>

namespace kugou
{
namespace httpproxy
{
namespace tokenserver
{
namespace handler
{

class CheckToken : public kugou::base::net::ImplHandler<kugou::httpproxy::tokenserver::bean::CheckTokenRequest,\
                        kugou::httpproxy::tokenserver::bean::CheckTokenResponse>
{
public:
    CheckToken(void)
    {
    }
	std::string name()
	{
		return CHECKTOKENREQUEST_API;
	}
    CheckToken* clone(void);
protected:
	virtual bool impl_handle();
	virtual bool impl_check();
};

}
}
}
}

#endif
