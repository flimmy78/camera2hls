/**
 * Title: Business Object Defination
 * Description: 
 * Copyright: Copyright (c) 2013 Kugou (All rights reserved)
 * Company: Kugou
 * Author: zhiguangq
 * Reviser: <Please input reviser>
 * Date: 2013-10-11
 * Version: 1.0
 * History: [1.0 2013-10-11]
 */

#ifndef DELCHANNELREQUEST_BEAN_SERVER_KUGOU_H
#define DELCHANNELREQUEST_BEAN_SERVER_KUGOU_H

#include <string>
#include <vector>
#include <utility>
#include <boost/shared_ptr.hpp>
#include <kugou/base/facility/Json.h>
#include <kugou/httpproxy/liveencoder/bean/APIDefine.h>


namespace kugou
{
namespace httpproxy
{
namespace liveencoder
{
namespace bean
{

struct DelChannelRequest : public kugou::base::facility::Json
{
    std::string Source_ID;

private:
	void assign()
	{
		JSON_ADD_STRING(Source_ID);
	}
};

} //bean
} //server
} //http
} //kugou

#endif
