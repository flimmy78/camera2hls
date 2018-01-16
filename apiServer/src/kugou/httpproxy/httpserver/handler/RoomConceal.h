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


#ifndef ROOMCONCEAL_HANDLER_HTTPSERVER_HTTPPROXY_KUGOU
#define ROOMCONCEAL_HANDLER_HTTPSERVER_HTTPPROXY_KUGOU

#include <string>

#include <kugou/base/net/ImplHandler.h>
#include <kugou/httpproxy/httpserver/bean/RoomConcealRequest.h>
#include <kugou/httpproxy/httpserver/bean/RoomConcealResponse.h>

namespace kugou
{
namespace httpproxy
{
namespace httpserver
{
namespace handler
{

class RoomConceal : public kugou::base::net::ImplHandler<kugou::httpproxy::httpserver::bean::RoomConcealRequest,\
                        kugou::httpproxy::httpserver::bean::RoomConcealResponse>
{
public:
    RoomConceal(void)
    {
    }
	std::string name()
	{
		return ROOMCONCEALREQUEST_API;
	}
    RoomConceal* clone(void);
protected:
	virtual bool impl_handle();
	virtual bool impl_check();
};

}
}
}
}

#endif
