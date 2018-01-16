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

#ifndef RELAYROOMSTRATEGY_BEAN_ACCESSSERVER_HTTPPROXY_KUGOU_H
#define RELAYROOMSTRATEGY_BEAN_ACCESSSERVER_HTTPPROXY_KUGOU_H

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

struct RelayRoomStrategy : public kugou::base::facility::Json
{
    int                         Enable;
	int							Mod;
    std::string                 RelayIP;
    std::string                 RelayPort;
    std::vector<std::string>    RelayRoomList;

    RelayRoomStrategy(void)
        : Enable(0)
    {
    }

private:
	void assign()
	{
        JSON_ADD_INT(Enable);
		JSON_ADD_INT(Mod);
        JSON_ADD_STRING(RelayIP);
        JSON_ADD_STRING(RelayPort);
        JSON_ADD_OBJECT_STRING(RelayRoomList);
	}
};

}
}
}
}

#endif
