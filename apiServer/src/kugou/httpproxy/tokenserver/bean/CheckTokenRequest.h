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

#ifndef CHECKTOKENREQUEST_BEAN_TOKENSERVER_HTTPPROXY_KUGOU_H
#define CHECKTOKENREQUEST_BEAN_TOKENSERVER_HTTPPROXY_KUGOU_H

#include <string>
#include <kugou/base/facility/Json.h>
#include <kugou/httpproxy/tokenserver/bean/APIDefine.h>

namespace kugou
{
namespace httpproxy
{
namespace tokenserver
{
namespace bean
{

struct CheckTokenRequest : public kugou::base::facility::Json
{
    std::string     userid;
    std::string     roomid;
    std::string     streamid;
    std::string     ip;
    std::string     action;
    std::string     token;    

private:
	void assign()
	{
        JSON_ADD_STRING(userid);
        JSON_ADD_STRING(roomid);
        JSON_ADD_STRING(streamid);
        JSON_ADD_STRING(ip);
        JSON_ADD_STRING(action);
        JSON_ADD_STRING(token);
	}
};

}
}
}
}

#endif
