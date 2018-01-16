/**
 * Title: HTTP Server
 * Description: 定义HTTP头信息
 * Copyright: Copyright (c) 2013 Kugou (All rights reserved)
 * Company: Kugou
 * Author: zhiguangq
 * Reviser: <Please input reviser>
 * Date: 2013-9-24
 * Version: 1.0
 * History: [1.0 2013-9-24]
 */
 
#ifndef HTTPHEADER_NET_BASE_KUGOU_H
#define HTTPHEADER_NET_BASE_KUGOU_H

#include <iostream>

namespace kugou
{
namespace base
{
namespace net
{

class HTTPHeader
{
public:
    std::string m_name;
    std::string m_value;
};
    
} // http    
} // base
} // net

#endif
