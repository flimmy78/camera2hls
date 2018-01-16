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

#ifndef IPSEARCHRESPONSE_BEAN_ACCESSSERVER_HTTPPROXY_KUGOU_H
#define IPSEARCHRESPONSE_BEAN_ACCESSSERVER_HTTPPROXY_KUGOU_H

#include <string>
#include <vector>
#include <kugou/base/facility/Json.h>
#include <kugou/httpproxy/accessserver/bean/GetAddressListResponse.h>

namespace kugou
{
namespace httpproxy
{
namespace accessserver
{
namespace bean
{
struct IPSearchResponse : public kugou::base::facility::Json
{
    int     Result;
    std::string Carrier;
    std::string Area;
    std::vector<struct AddressListObject> AddressList;

private:
	void assign()
	{
        JSON_ADD_INT(Result);
        JSON_ADD_STRING(Carrier);
        JSON_ADD_STRING(Area);
        JSON_ADD_OBJECT(AddressList);
	}
};

}
}
}
}

#endif
