/**
 * Title: manage all encoder
 * Description: 
 * Copyright: Copyright (c) 2013 Kugou (All rights reserved)
 * Company: Kugou
 * Author: zhiguangq
 * Reviser: <Please input reviser>
 * Date: 2013-10-23
 * Version: 1.0
 * History: [1.0 2013-10-23]
 */

#ifndef SCHEDULEENCODELIBRARY_HANDLE_SERVER_KUGOU_H
#define SCHEDULEENCODELIBRARY_HANDLE_SERVER_KUGOU_H

#include <string>
#include <map>

#include <kugou/httpproxy/liveencoder/bean/EncoderHeartbeatRequest.h>
#include <kugou/httpproxy/liveencoder/encoder/LibraryImpl.h>

#include <boost/thread/mutex.hpp>
#include <Poco/ThreadPool.h>
#include <Poco/Timestamp.h>

namespace kugou
{
namespace httpproxy
{
namespace liveencoder
{
namespace scheduleencoder
{

class ScheduleEncoderLibrary : public kugou::httpproxy::liveencoder::encoder::LibraryImpl, public Poco::Runnable
{
    struct EncoderInfo
    {
        int             m_cpuUsage;
        int             m_tasks;
        Poco::Timestamp m_updateTimestamp;
        EncoderInfo(int usage = 100, int task = MAX_TASK)
        {
            m_cpuUsage = usage;
            m_tasks    = task;
        }
    };
    typedef std::map<std::string, EncoderInfo> EncoderMap;
    typedef std::map<std::string, std::string> StreamIDEncoderMAP;
    static const   int MAX_TASK = 32;
public:
    ScheduleEncoderLibrary(void);
    ~ScheduleEncoderLibrary();

    void addEncoder(std::string encoderID, std::vector<std::string>& idVector);
    void delEncoder(std::string encoderID);

    void onHeartbeat(kugou::httpproxy::liveencoder::bean::EncoderHeartbeatRequest& request);
    virtual void addEncodeStreamIDSuccess(std::string id);

    virtual bool addEncodeStreamID(std::string id, std::string encoderID = "");
    virtual void delEncodeStreamID(std::string id);

    virtual void run();
private:
    void cleanEncoderStreamID(std::string encoderID);

    boost::mutex        m_mutexEncoderUsageMap;
    EncoderMap     m_encoderMap;

    boost::mutex        m_mutexStreamIDEncoderMAP;
    StreamIDEncoderMAP  m_streamIDEncoderMAP;

    Poco::Thread        m_heartbeatTimeoutThread;
    bool                m_isHeartbeatTimeoutThreadStop;
};

}
}
}
}

#endif
