/**
 * Title: Lunch encoder thread
 * Description: 
 * Copyright: Copyright (c) 2013 Kugou (All rights reserved)
 * Company: Kugou
 * Author: zhiguangq
 * Reviser: <Please input reviser>
 * Date: 2013-10-8
 * Version: 1.0
 * History: [1.0 2013-10-8]
 */


#ifndef ENCODERWORKER_HANDLE_SERVER_KUGOU_H
#define ENCODERWORKER_HANDLE_SERVER_KUGOU_H

#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>
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

class EncoderWorker : public Poco::Runnable
{
public:
    EncoderWorker(std::string id, std::string& src, std::string& dst);

    virtual void run();
    void stop(void);
    bool isStop(void);
private:
    std::string m_encodeID;
    boost::shared_ptr<Poco::ProcessHandle> m_processHandler;
    boost::mutex        m_mutexProcessHandler;

    int                 m_retryTime;
    std::string         m_srcStreamServer;
    std::string         m_dstStreamServer;
    bool                m_isStop;
};

}
}
}
}

#endif
