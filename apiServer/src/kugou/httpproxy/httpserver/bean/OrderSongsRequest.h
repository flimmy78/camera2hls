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

#ifndef ORDERSONGSREQUEST_BEAN_HTTPSERVER_HTTPPROXY_KUGOU
#define ORDERSONGSREQUEST_BEAN_HTTPSERVER_HTTPPROXY_KUGOU

#include <string>
#include <vector>
#include <utility>
#include <boost/shared_ptr.hpp>
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

struct OrderSongsRequest : public kugou::base::facility::Json
{
    std::string     Source_ID;
    std::string     Choosed_IP;

private:
	void assign()
	{
        JSON_ADD_STRING(Source_ID);
        JSON_ADD_STRING(Choosed_IP);
	}
};

} //bean
} //httpserver
} //httpproxy
} //kugou

#endif
