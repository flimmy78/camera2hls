/**
 * Title: Interface Handler
 * Description: Handler add encode channel api interface
 * Copyright: Copyright (c) 2013 Kugou (All rights reserved)
 * Company: Kugou
 * Author: zhiguangq
 * Reviser: <Please input reviser>
 * Date: 2013-9-24
 * Version: 1.0
 * History: [1.0 2013-9-24]
 */


#ifndef IPSEARCH_HANDLER_ACCESSSERVER_HTTPPROXY_KUGOU_H
#define IPSEARCH_HANDLER_ACCESSSERVER_HTTPPROXY_KUGOU_H

#include <string>

#include <kugou/base/net/ImplHandler.h>
#include <kugou/httpproxy/accessserver/bean/IPSearchRequest.h>
#include <kugou/httpproxy/accessserver/bean/IPSearchResponse.h>

namespace kugou
{
namespace httpproxy
{
namespace accessserver
{
namespace handler
{

class IPSearch : public kugou::base::net::ImplHandler<kugou::httpproxy::accessserver::bean::IPSearchRequest,\
                        kugou::httpproxy::accessserver::bean::IPSearchResponse>
{
public:
    IPSearch(void)
    {
    }
	std::string name()
	{
		return IPSEARCH_API;
	}
    IPSearch* clone(void);
protected:
    virtual void getPeerAddress(const std::string& peerAddress);
	virtual bool impl_handle();
	virtual bool impl_check();
};

}
}
}
}

#endif
