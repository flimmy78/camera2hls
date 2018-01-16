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

#ifndef CREATETOKENRESPONSE_BEAN_TOKENSERVER_HTTPPROXY_KUGOU
#define CREATETOKENRESPONSE_BEAN_TOKENSERVER_HTTPPROXY_KUGOU

#include <string>
#include <vector>
#include <kugou/base/facility/Json.h>

namespace kugou
{
namespace httpproxy
{
namespace tokenserver
{
namespace bean
{

struct CreateTokenResponse : public kugou::base::facility::Json
{
    std::string     result;
    std::string     token;

private:
	void assign()
	{
        JSON_ADD_STRING(result);
        JSON_ADD_STRING(token);
	}
};

}
}
}
}

#endif
