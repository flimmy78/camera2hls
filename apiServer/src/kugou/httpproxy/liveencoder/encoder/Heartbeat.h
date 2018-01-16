/**
 * Title: Heart beat
 * Description: 
 * Copyright: Copyright (c) 2013 Kugou (All rights reserved)
 * Company: Kugou
 * Author: zhiguangq
 * Reviser: <Please input reviser>
 * Date: 2013-10-21
 * Version: 1.0
 * History: [1.0 2013-10-21]
 */


#ifndef HEARTBEAT_HANDLE_SERVER_KUGOU_H
#define HEARTBEAT_HANDLE_SERVER_KUGOU_H

#include <string>

#include <kugou/base/net/HTTPClient.h>
#include <kugou/base/facility/SysHealth.h>
#include <kugou/httpproxy/liveencoder/encoder/EncoderLibrary.h>

#include <Poco/Process.h>
#include <Poco/Runnable.h>

namespace kugou
{
namespace httpproxy
{
namespace liveencoder
{
namespace encoder
{

class Heartbeat : public Poco::Runnable
{
public:
    Heartbeat(kugou::httpproxy::liveencoder::encoder::EncoderLibrary& el);
    ~Heartbeat();

    void setScheduleServer(const std::string& ip, const std::string& port);
    void setEncoderLocal(const std::string& ip, const std::string& port);

    virtual void run();
private:
    bool m_isStop;
    kugou::base::facility::SysHealth::SysHealthPtr  m_pSysHealth;
    kugou::httpproxy::liveencoder::encoder::EncoderLibrary&         m_encoderLibrary;

    std::string     m_scheduleServerIP;
    std::string     m_scheduleServerPort;
    
    std::string     m_encoderLocalIP;
    std::string     m_encoderLocalPort;
};

}
}
}
}

#endif
