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

#ifndef GETADDRESSLISTRESPONSE_BEAN_ACCESSSERVER_HTTPPROXY_KUGOU_H
#define GETADDRESSLISTRESPONSE_BEAN_ACCESSSERVER_HTTPPROXY_KUGOU_H

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
struct AddressListObject : public kugou::base::facility::Json
{
    std::string Addr;
    std::string Port;
private:
    void assign()
    {
        JSON_ADD_STRING(Addr);
        JSON_ADD_STRING(Port);
    }
};

struct GetAddressListResponse : public kugou::base::facility::Json
{
    int     Result;
    std::vector<struct AddressListObject> AddressList;

private:
	void assign()
	{
        JSON_ADD_INT(Result);
        JSON_ADD_OBJECT(AddressList);
	}
};

}
}
}
}

#endif
