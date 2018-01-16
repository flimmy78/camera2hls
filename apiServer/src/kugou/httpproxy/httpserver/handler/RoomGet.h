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


#ifndef ROOMGET_HANDLER_HTTPSERVER_HTTPPROXY_KUGOU
#define ROOMGET_HANDLER_HTTPSERVER_HTTPPROXY_KUGOU

#include <string>

#include <kugou/base/net/ImplHandler.h>
#include <kugou/httpproxy/httpserver/bean/RoomGetRequest.h>
#include <kugou/httpproxy/httpserver/bean/RoomGetResponse.h>

namespace kugou
{
namespace httpproxy
{
namespace httpserver
{
namespace handler
{

class RoomGet : public kugou::base::net::ImplHandler<kugou::httpproxy::httpserver::bean::RoomGetRequest,\
                        kugou::httpproxy::httpserver::bean::RoomGetResponse>
{
public:
    RoomGet(void)
    {
    }
	std::string name()
	{
		return ROOMGETREQUEST_API;
	}
    RoomGet* clone(void);
protected:
	virtual bool impl_handle();
	virtual bool impl_check();
};

}
}
}
}

#endif
