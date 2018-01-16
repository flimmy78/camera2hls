/**
 * Title: Interface Handler
 * Description: Handler add encode channel api interface
 * Copyright: Copyright (c) 2013 Kugou (All rights reserved)
 * Company: Kugou
 * Author: zhiguangq
 * Reviser: <Please input reviser>
 * Date: 2013-10-11
 * Version: 1.0
 * History: [1.0 2013-10-11]
 */


#ifndef DELCHANNEL_HANDLE_SERVER_KUGOU_H
#define DELCHANNEL_HANDLE_SERVER_KUGOU_H

#include <string>

#include <kugou/base/net/ImplHandler.h>
#include <kugou/httpproxy/liveencoder/bean/DelEncodeChannelRequest.h>
#include <kugou/httpproxy/liveencoder/bean/DelEncodeChannelResponse.h>
#include <kugou/httpproxy/liveencoder/encoder/LibraryImpl.h>

namespace kugou
{
namespace httpproxy
{
namespace liveencoder
{
namespace handler
{

class DelEncodeChannel : public kugou::base::net::ImplHandler<kugou::httpproxy::liveencoder::bean::DelChannelRequest, kugou::httpproxy::liveencoder::bean::DelChannelResponse>
{
public:
    DelEncodeChannel(kugou::httpproxy::liveencoder::encoder::LibraryImpl& el)
        : m_el(el)
    {
    }
	std::string name()
	{
		return DELCHANNELREQUEST_API;
	}
protected:
	virtual bool impl_handle();
	virtual bool impl_check();
    virtual Handler* clone(void)
    {
        return new DelEncodeChannel(m_el);
    }

private:
    kugou::httpproxy::liveencoder::encoder::LibraryImpl& m_el;
};

}
}
}
}

#endif
