/**
 * Title: HTTP Connection
 * Description: 处理一个HTTP connection
 * Copyright: Copyright (c) 2013 Kugou (All rights reserved)
 * Company: Kugou
 * Author: zhiguangq
 * Reviser: <Please input reviser>
 * Date: 2013-9-24
 * Version: 1.0
 * History: [1.0 2013-9-24]
 */

#ifndef HTTPCONNECTION_NET_BASE_KUGOU_H
#define HTTPCONNECTION_NET_BASE_KUGOU_H

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <kugou/base/net/HTTPReply.h>
#include <kugou/base/net/HTTPRequestHandler.h>

namespace kugou
{
namespace base
{
namespace net
{

class HTTPConnectionManager;

class HTTPConnection
    : public boost::enable_shared_from_this<HTTPConnection>
    , private boost::noncopyable
{
public:
    /// Construct a connection with the given io_service.
    explicit HTTPConnection(boost::asio::io_service& io_service,
        HTTPConnectionManager& manager, HTTPRequestHandler& handler);

	/**
	 * Get the socket associated with the connection.
	 *
	 */    
    boost::asio::ip::tcp::socket& socket();

	/**
	 * Start the first asynchronous operation for the connection.
	 *
	 */    
    void start();

	/**
	 * Stop all asynchronnous operations with the connection.
	 *
	 */   
    void stop();
private:
    /// Handle completion of a read operation.
    void handleRead(const boost::system::error_code& e, std::size_t bytes_transferrd);

    /// Handle completion of a write operation.
    void handleWrite(const boost::system::error_code& e);

    /// Socket for the connection.
    boost::asio::ip::tcp::socket m_socket;

    /// The manager for this connection.
    HTTPConnectionManager& m_connectionManager;

    /// The handler used to process the incoming request.
    HTTPRequestHandler& m_requestHandler;

    /// Buffer for incoming data.
    boost::array<char, 8192> m_buffer;

    /// The parser for the incoming request.
    //HTTPRequestParser m_requestParser;

    /// The reply to be sent back to the client.
    HTTPReply m_reply;
};

typedef boost::shared_ptr<HTTPConnection> ConnectionSharePtr;

} // kugou
} // base
} // net

#endif // HTTPCONNECTION_NET_BASE_KUGOU_H
