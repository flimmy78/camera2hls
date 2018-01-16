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

#ifndef ENCODING_BEAN_ACCESSSERVER_HTTPPROXY_KUGOU_H
#define ENCODING_BEAN_ACCESSSERVER_HTTPPROXY_KUGOU_H

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
struct EncodingMapObject : public kugou::base::facility::Json
{
    std::string en;
    std::string cn;
private:
    void assign()
    {
        JSON_ADD_STRING(en);
        JSON_ADD_STRING(cn);
    }
};
struct Encoding : public kugou::base::facility::Json
{
    std::vector<struct EncodingMapObject> EncodingMap;
private:
	void assign()
	{
        JSON_ADD_OBJECT(EncodingMap);
	}
};

}
}
}
}

#endif
