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

#ifndef ADDCHANNELRESPONSE_BEAN_SERVER_KUGOU_H
#define ADDCHANNELRESPONSE_BEAN_SERVER_KUGOU_H

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

#include <kugou/base/facility/Json.h>

namespace kugou
{
namespace httpproxy
{
namespace liveencoder
{
namespace bean
{

struct AddChannelResponse : public kugou::base::facility::Json
{
	std::string Result;

private:
	void assign()
	{
		JSON_ADD_STRING(Result);
	}
};

} //bean
} //server
} //http
} //kugou

#endif
