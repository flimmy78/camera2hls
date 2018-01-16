#include <kugou/base/net/StreamSocket.h>
#include <kugou/base/facility/ErrorMsg.h>
#include <kugou/base/facility/Exception.h>
#include <kugou/base/facility/log.h>

#include <iostream>

using namespace kugou::base::facility;

namespace kugou
{
namespace base
{
namespace net
{
EXCEPTION(SocketException, kugou::base::facility::Exception);      // SocketException
EXCEPTION(BindFailureException, kugou::base::facility::Exception);   // BindFailureException
EXCEPTION(FCNTLException, kugou::base::facility::Exception);      // FCNTLException

StreamSocket::StreamSocket(std::string ip, int port, int timeout, int bufferSize)
	: m_ip(ip)
	, m_port(port)
{	
	m_bSockListening = false;
	this->bind();
}

void StreamSocket::bind(void)
{	
    m_s = socket(AF_INET, SOCK_STREAM, 0);
    if (m_s == INVALID_SOCKET)
    {
        throw SocketException(getErrorMsg());
    }	
	
	int one = 1;
	setsockopt(m_s, SOL_SOCKET, SO_REUSEADDR, (char *)&one, sizeof(int));

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(m_ip.c_str());//INADDR_ANY;
	addr.sin_port = htons(m_port);
	
	if(::bind(m_s, (struct sockaddr*)&addr, sizeof(addr)) < 0)
	{
		LOG_E("Failed to Bind socket, Error: " << getErrorMsg());
		return;
	}
	
	if (::listen(m_s, 10240) < 0)
	{
		LOG_E("Failed to Listen socket, Error: " << getErrorMsg());
		return;
	}

	m_bSockListening = true;
	
    // 把socket设置成非阻塞的
#if defined WIN32
    u_long flag = 1;
    if(ioctlsocket(m_s, FIONBIO, &flag) < 0)
#elif defined __sun__ || defined __linux__
    int flags;
	if ((flags = fcntl(m_s, F_GETFL, 0)) < 0 || fcntl(m_s, F_SETFL, flags | O_NONBLOCK) < 0)
#endif
	{
		LOG_E("Failed to set socket FIONBIO, Error: " << getErrorMsg());
		return;
	}
}

int StreamSocket::getStreamSocket(void)
{
    return m_s;
}

StreamSocket::~StreamSocket()
{

}

bool StreamSocket::isSockListening(void)
{
	return m_bSockListening;
}


}
}
}

