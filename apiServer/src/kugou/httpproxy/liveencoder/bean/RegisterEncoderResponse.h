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

#ifndef REGISTERENCODERRESPONSE_BEAN_SERVER_KUGOU_H
#define REGISTERENCODERRESPONSE_BEAN_SERVER_KUGOU_H

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

struct RegisterEncoderResponse : public kugou::base::facility::Json
{
	std::string Result;
    std::vector< std::string > StreamIDList;

private:
	void assign()
	{
		JSON_ADD_STRING(Result);
        JSON_ADD_OBJECT_STRING(StreamIDList);
	}
};

} //bean
} //server
} //http
} //kugou

#endif /* ADDCHANNELRESPONSE_BEAN_SERVER_KUGOU_H */
