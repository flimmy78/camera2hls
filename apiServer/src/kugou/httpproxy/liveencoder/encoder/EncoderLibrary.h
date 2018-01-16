/**
 * Title: manage encoder
 * Description: 
 * Copyright: Copyright (c) 2013 Kugou (All rights reserved)
 * Company: Kugou
 * Author: zhiguangq
 * Reviser: <Please input reviser>
 * Date: 2013-10-11
 * Version: 1.0
 * History: [1.0 2013-10-11]
 */


#ifndef ENCODELIBRARY_HANDLE_SERVER_KUGOU_H
#define ENCODELIBRARY_HANDLE_SERVER_KUGOU_H

#include <string>
#include <map>

#include <kugou/httpproxy/liveencoder/encoder/LibraryImpl.h>
#include <kugou/httpproxy/liveencoder/encoder/EncoderWorker.h>
#include <boost/thread/mutex.hpp>
#include <Poco/ThreadPool.h>

namespace kugou
{
namespace httpproxy
{
namespace liveencoder
{
namespace encoder
{

class EncoderLibrary : public LibraryImpl
{
    typedef std::map<std::string, EncoderWorker*> EncoderWorkerMAP;
public:
    EncoderLibrary(Poco::ThreadPool& tp, std::string& src, std::string& dst);

    void getStreamIDs(std::vector<std::string>& idVector);

    virtual bool addEncodeStreamID(std::string id, std::string encoderID = "");
    virtual void delEncodeStreamID(std::string id);
    virtual void addEncodeStreamIDSuccess(std::string id){}

public:
    Poco::ThreadPool& m_threadPool;

    EncoderWorkerMAP    m_encoderWorkerMap;
    boost::mutex        m_mutexEncoderWorkerMap;

    std::string         m_srcStreamServer;
    std::string         m_dstStreamServer;
};

}
}
}
}

#endif
