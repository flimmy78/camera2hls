#ifndef EXCEPTION_NET_BASE_KUGOU_H
#define EXCEPTION_NET_BASE_KUGOU_H

#ifdef __cplusplus
extern "C"{
#endif
#include <event.h>
#include <evhttp.h>
#include <event2/buffer.h>
#include <event2/util.h>
#include <event2/keyvalq_struct.h>
#ifdef __cplusplus
}
#endif

#include <boost/scoped_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/scoped_ptr.hpp>

#include <kugou/base/net/StreamSocket.h>
#include <kugou/base/net/HTTPConnectionManager.h>

#define THREAD_SIZE 10

namespace kugou
{
namespace base
{
namespace net
{

class HTTPServer
{
public:
    HTTPServer(int threads);
    ~HTTPServer();

    /// Register Handler
    void registerHandler(kugou::base::net::Handler* handler);

    int serv(std::string ip, int port);
protected:
    static void* Dispatch(void *arg);
    static void GenericHandler(struct evhttp_request *req, void *arg);
    static void onConnectionClose(struct evhttp_connection *, void *);
    int BindSocket(int port);

private:
    boost::scoped_ptr <StreamSocket> m_listenedSocket;

    int     m_threadNumber;
    boost::scoped_ptr<boost::thread>            m_thread[THREAD_SIZE];
    boost::scoped_ptr<HTTPConnectionManager>    m_HTTPConnectionManager[THREAD_SIZE];
};

}
}
}

#endif  // EXCEPTION_BASE_NET_KUGOU_H

