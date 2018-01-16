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


#ifndef GETADDRESSLIST_HANDLER_ACCESSSERVER_HTTPPROXY_KUGOU_H
#define GETADDRESSLIST_HANDLER_ACCESSSERVER_HTTPPROXY_KUGOU_H

#include <string>

#include <kugou/base/net/ImplHandler.h>
#include <kugou/httpproxy/accessserver/bean/GetAddressListRequest.h>
#include <kugou/httpproxy/accessserver/bean/GetAddressListResponse.h>

namespace kugou
{
namespace httpproxy
{
namespace accessserver
{
namespace handler
{

class GetAddressList : public kugou::base::net::ImplHandler<kugou::httpproxy::accessserver::bean::GetAddressListRequest,\
                        kugou::httpproxy::accessserver::bean::GetAddressListResponse>
{
public:
    GetAddressList(void)
    {
    }
	std::string name()
	{
		return GETADDRESSLIST_API;
	}
    GetAddressList* clone(void);
protected:
    virtual void getPeerAddress(const std::string& peerAddress);
	virtual bool impl_handle();
	virtual bool impl_check();

private:
    std::string m_pearAddress;
};

}
}
}
}

#endif
