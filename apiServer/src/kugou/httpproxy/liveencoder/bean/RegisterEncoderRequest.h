/**
 * Title: Business Object Defination
 * Description: 
 * Copyright: Copyright (c) 2013 Kugou (All rights reserved)
 * Company: Kugou
 * Author: zhiguangq
 * Reviser: <Please input reviser>
 * Date: 2013-10-23
 * Version: 1.0
 * History: [1.0 2013-10-23]
 */

#ifndef REGISTERENCODERREQUEST_BEAN_SERVER_KUGOU_H
#define REGISTERENCODERREQUEST_BEAN_SERVER_KUGOU_H

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

struct RegisterEncoderRequest : public kugou::base::facility::Json
{
    std::string Encoder_ID;

private:
	void assign()
	{
		JSON_ADD_STRING(Encoder_ID);
	}
};

} //bean
} //server
} //http
} //kugou

#endif
