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

#ifndef ROOMGETREQUEST_BEAN_HTTPSERVER_HTTPPROXY
#define ROOMGETREQUEST_BEAN_HTTPSERVER_HTTPPROXY

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

struct RoomGetRequest : public kugou::base::facility::Json
{
    int             Sex;
    int             LastRoomID;

    RoomGetRequest(void)
        : Sex(-1)
        , LastRoomID(0)
    {
    }

private:
	void assign()
	{
        JSON_ADD_INT(Sex);
        JSON_ADD_INT(LastRoomID);
	}
};

}
}
}
}

#endif
