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

#ifndef CREATETOKENREQUEST_BEAN_TOKENSERVER_HTTPPROXY_KUGOU
#define CREATETOKENREQUEST_BEAN_TOKENSERVER_HTTPPROXY_KUGOU

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

struct CreateTokenRequest : public kugou::base::facility::Json
{
    std::string     userid;
    std::string     roomid;
    std::string     streamid;
    std::string     ip;
    std::string     action;
    std::string     time;      // 这个参数在request中是不存在的，只是为了生成和解析token用的
    std::string		effectivetime;   // 这个参数是为了对线下直播提供token用，可以设置token的有效时间

private:
	void assign()
	{
        JSON_ADD_STRING(userid);
        JSON_ADD_STRING(roomid);
        JSON_ADD_STRING(streamid);
        JSON_ADD_STRING(ip);
        JSON_ADD_STRING(action);
        JSON_ADD_STRING(time);
        JSON_ADD_STRING(effectivetime);
	}
};

}
}
}
}

#endif
