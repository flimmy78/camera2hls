#include <algorithm>

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

#include <boost/bind.hpp>
#include <boost/thread.hpp>

#include <kugou/base/facility/log.h>
#include <kugou/base/net/HTTPConnectionManager.h>

namespace kugou
{
namespace base
{
namespace net
{
HTTPConnectionManager::HTTPConnectionManager()
    : m_requestHandler(new HTTPRequestHandler(""))
{
}

void HTTPConnectionManager::start(ConnectionSharePtr c)
{
    this->m_connections.insert(c);
    c->start();
}

void HTTPConnectionManager::stop(ConnectionSharePtr c)
{
    this->m_connections.erase(c);
    c->stop();
}

void HTTPConnectionManager::stopAll()
{
    std::for_each(this->m_connections.begin(),this->m_connections.end(),boost::bind(&HTTPConnection::stop, _1));

    this->m_connections.clear();
}

// ��ͬ���̣߳����������ӿ�
void HTTPConnectionManager::newRequest(void* req, std::string uri, std::string content)
{
    HTTPReply m_reply;

    // ȡ���Զ˵�IP
    ev_uint16_t port;
    char *addr = NULL;
    evhttp_connection_get_peer(((evhttp_request*)req)->evcon, &addr , &port);
    char address[20];
    strcpy(address, addr);

    LOG_D("peerAddress= ["<< address << ":" << port  << "] uri=" << uri);

    m_requestHandler->handleRequest(address, uri, content, m_reply);

    //if(m_reply.status ==  > 0)    // �����request������ˣ��ͻظ�
    {
        struct evbuffer *buf = evbuffer_new();
        evbuffer_add_printf(buf, "%s", m_reply.m_content.c_str());
		evhttp_add_header(evhttp_request_get_output_headers((evhttp_request*)req), "Connection", "close");
        evhttp_add_header(evhttp_request_get_output_headers((evhttp_request*)req), \
            m_reply.m_headers[0].m_name.c_str(), m_reply.m_headers[0].m_value.c_str());
        evhttp_send_reply((evhttp_request*)req, HTTP_OK, "OK", buf);
		evbuffer_free(buf);

        LOG_D("<== [" << m_reply.m_content << "]");
    }
}

} // http    
} // server
} // kugou
