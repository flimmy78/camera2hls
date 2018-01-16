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

#ifndef ROOMPUBLICRESPONSE_BEAN_HTTPSERVER_HTTPPROXY_KUGOU
#define ROOMPUBLICRESPONSE_BEAN_HTTPSERVER_HTTPPROXY_KUGOU

#include <string>
#include <vector>
#include <kugou/base/facility/Json.h>

namespace kugou
{
namespace httpproxy
{
namespace httpserver
{
namespace bean
{

struct RoomPublicResponse : public kugou::base::facility::Json
{
    std::string         Result;
    std::string         Description;
private:
	void assign()
	{
        JSON_ADD_STRING(Result);
        JSON_ADD_STRING(Description);
	}
};

}
}
}
}

#endif
