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

#ifndef IPSEARCHEQUEST_BEAN_ACCESSSERVER_HTTPPROXY_KUGOU_H
#define IPSEARCHEQUEST_BEAN_ACCESSSERVER_HTTPPROXY_KUGOU_H

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

struct IPSearchRequest : public kugou::base::facility::Json
{
    std::string     Addr;

private:
	void assign()
	{
        JSON_ADD_STRING(Addr);
	}
};

}
}
}
}

#endif
