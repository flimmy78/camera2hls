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


#ifndef ADDENCODECHANNELSUCCESS_HANDLE_SERVER_KUGOU_H
#define ADDENCODECHANNELSUCCESS_HANDLE_SERVER_KUGOU_H

#include <string>

#include <kugou/base/net/ImplHandler.h>
#include <kugou/httpproxy/liveencoder/bean/AddEncodeChannelRequest.h>
#include <kugou/httpproxy/liveencoder/bean/AddEncodeChannelResponse.h>
#include <kugou/httpproxy/liveencoder/encoder/LibraryImpl.h>
#include <kugou/httpproxy/liveencoder/bean/APIDefine.h>

namespace kugou
{
namespace httpproxy
{
namespace liveencoder
{
namespace handler
{

class AddEncodeChannelSuccess : public kugou::base::net::ImplHandler<kugou::httpproxy::liveencoder::bean::AddChannelRequest, kugou::httpproxy::liveencoder::bean::AddChannelResponse>
{
public:
    AddEncodeChannelSuccess(kugou::httpproxy::liveencoder::encoder::LibraryImpl& el)
        : m_el(el)
    {
    }
	std::string name()
	{
		return ADDCHANNELSUCCESSREQUEST_API;
	}
protected:
	virtual bool impl_handle();
	virtual bool impl_check();
    virtual Handler* clone(void)
    {
        return new AddEncodeChannelSuccess(m_el);
    }

public:
    kugou::httpproxy::liveencoder::encoder::LibraryImpl& m_el;
};

}
}
}
}

#endif
