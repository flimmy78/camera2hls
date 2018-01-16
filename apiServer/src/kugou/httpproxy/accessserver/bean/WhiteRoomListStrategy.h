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

#ifndef WHITEROOMLISTSTRATEGY_BEAN_ACCESSSERVER_HTTPPROXY_KUGOU_H
#define WHITEROOMLISTSTRATEGY_BEAN_ACCESSSERVER_HTTPPROXY_KUGOU_H

#include <string>
#include <kugou/base/facility/Json.h>

namespace kugou
{
namespace httpproxy
{
namespace accessserver
{
namespace bean
{

struct WhiteRoomListStrategy : public kugou::base::facility::Json
{
    std::vector<std::string>    WhiteRoomList;

private:
	void assign()
	{
        JSON_ADD_OBJECT_STRING(WhiteRoomList);
	}
};

}
}
}
}

#endif
