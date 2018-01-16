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

#ifndef GETTESTNODELISTRESPONSE_BEAN_ACCESSSERVER_HTTPPROXY_KUGOU_H
#define GETTESTNODELISTRESPONSE_BEAN_ACCESSSERVER_HTTPPROXY_KUGOU_H

#include <string>
#include <vector>
#include <kugou/base/facility/Json.h>

namespace kugou
{
namespace httpproxy
{
namespace accessserver
{
namespace bean
{
struct NodeListObject : public kugou::base::facility::Json
{
    std::string ID;
    std::string Addr;
    std::string Port;
    std::string Carrier;
    std::string Area;
private:
    void assign()
    {
        JSON_ADD_STRING(ID);
        JSON_ADD_STRING(Addr);
        JSON_ADD_STRING(Port);
        JSON_ADD_STRING(Carrier);
        JSON_ADD_STRING(Area);
    }
};

struct GetTestNodeListResponse : public kugou::base::facility::Json
{
    int     Result;
    std::string  UserAddr;
    std::string  UserCarrier;
    std::string  UserArea;
    std::vector<struct NodeListObject> NodeList;

private:
	void assign()
	{
        JSON_ADD_INT(Result);
        JSON_ADD_STRING(UserAddr);
        JSON_ADD_STRING(UserCarrier);
        JSON_ADD_STRING(UserArea);
        JSON_ADD_OBJECT(NodeList);
	}
};

}
}
}
}

#endif
