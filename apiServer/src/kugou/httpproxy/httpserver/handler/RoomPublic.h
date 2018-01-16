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


#ifndef ROOMPUBLIC_HANDLER_HTTPSERVER_HTTPPROXY_KUGOU
#define ROOMPUBLIC_HANDLER_HTTPSERVER_HTTPPROXY_KUGOU

#include <string>
#include <kugou/base/net/ImplHandler.h>
#include <kugou/httpproxy/httpserver/bean/RoomPublicRequest.h>
#include <kugou/httpproxy/httpserver/bean/RoomPublicResponse.h>

namespace kugou
{
namespace httpproxy
{
namespace httpserver
{
namespace handler
{

class RoomPublic : public kugou::base::net::ImplHandler<kugou::httpproxy::httpserver::bean::RoomPublicRequest,\
                        kugou::httpproxy::httpserver::bean::RoomPublicResponse>
{
public:
    RoomPublic(void)
    {
    }
	std::string name()
	{
		return ROOMPUBLICREQUEST_API;
	}
    RoomPublic* clone(void);
protected:
	virtual bool impl_handle();
	virtual bool impl_check();
};

}
}
}
}

#endif
