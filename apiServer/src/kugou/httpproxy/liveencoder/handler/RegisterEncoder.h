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


#ifndef REGISTERENCODER_HANDLE_SERVER_KUGOU_H
#define REGISTERENCODER_HANDLE_SERVER_KUGOU_H

#include <string>

#include <kugou/base/net/ImplHandler.h>
#include <kugou/httpproxy/liveencoder/bean/RegisterEncoderRequest.h>
#include <kugou/httpproxy/liveencoder/bean/RegisterEncoderResponse.h>
#include <kugou/httpproxy/liveencoder/scheduleencoder/ScheduleEncoderLibrary.h>

namespace kugou
{
namespace httpproxy
{
namespace liveencoder
{
namespace handler
{

class RegisterEncoder : public kugou::base::net::ImplHandler<kugou::httpproxy::liveencoder::bean::RegisterEncoderRequest\
                            , kugou::httpproxy::liveencoder::bean::RegisterEncoderResponse>
{
public:
    RegisterEncoder(kugou::httpproxy::liveencoder::scheduleencoder::ScheduleEncoderLibrary& sel)
        : m_sel(sel)
    {
    }
	std::string name()
	{
		return REGISTERENCODER_API;
	}
protected:
	virtual bool impl_handle();
	virtual bool impl_check();
    virtual Handler* clone(void)
    {
        return new RegisterEncoder(m_sel);
    }

public:
    kugou::httpproxy::liveencoder::scheduleencoder::ScheduleEncoderLibrary& m_sel;
};

}
}
}
}

#endif
