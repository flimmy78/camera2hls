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

#ifndef GETTESTNODELISTREQUEST_BEAN_ACCESSSERVER_HTTPPROXY_KUGOU_H
#define GETTESTNODELISTREQUEST_BEAN_ACCESSSERVER_HTTPPROXY_KUGOU_H

#include <string>
#include <kugou/base/facility/Json.h>
#include <kugou/httpproxy/accessserver/bean/APIDefine.h>

namespace kugou
{
namespace httpproxy
{
namespace accessserver
{
namespace bean
{

struct GetTestNodeListRequest : public kugou::base::facility::Json
{
    int             UsageType;
    int             UserType;
    std::string     UserID;
    std::string     RoomID;
	std::string     StarID;

    GetTestNodeListRequest(void)
        : UsageType(-1)
        , UserType(-1)
    {
    }

private:
	void assign()
	{
        JSON_ADD_INT(UsageType);
        JSON_ADD_INT(UserType);
        JSON_ADD_STRING(UserID);
        JSON_ADD_STRING(RoomID);
		JSON_ADD_STRING(StarID);
	}
};

}
}
}
}

#endif
