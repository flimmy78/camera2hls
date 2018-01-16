/**
 * Title: Business Object Defination
 * Description: 
 * Copyright: Copyright (c) 2013 Kugou (All rights reserved)
 * Company: Kugou
 * Author: zhiguangq
 * Reviser: <Please input reviser>
 * Date: 2013-9-24
 * Version: 1.0
 * History: [1.0 2013-9-24]
 */

#ifndef ROOMCONCEALREQUEST_BEAN_HTTPSERVER_HTTPPROXY_KUGOU
#define ROOMCONCEALREQUEST_BEAN_HTTPSERVER_HTTPPROXY_KUGOU

#include <string>
#include <kugou/base/facility/Json.h>
#include <kugou/httpproxy/httpserver/bean/APIDefine.h>

namespace kugou
{
namespace httpproxy
{
namespace httpserver
{
namespace bean
{

struct RoomConcealRequest : public kugou::base::facility::Json
{
    int             RoomID;
    std::string     RoomK;

    RoomConcealRequest(void)
        : RoomID(-1)
    {
    }

private:
	void assign()
	{
        JSON_ADD_INT(RoomID);
        JSON_ADD_STRING(RoomK);
	}
};

}
}
}
}

#endif
