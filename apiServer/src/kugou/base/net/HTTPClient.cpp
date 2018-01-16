#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>

#include <kugou/base/net/HTTPClient.h>
#include <kugou/base/facility/log.h>

namespace kugou
{
namespace base
{
namespace net
{

HTTPClient::HTTPClient(const std::string& ip, const std::string& port, const std::string& uri)
    : m_ioService()
    , m_resolver(m_ioService)
    , m_ip(ip)
    , m_port(port)
    , m_uri(uri)
{
}

bool HTTPClient::HTTPGet(std::string domin)
{
    try
    {
        boost::asio::ip::tcp::resolver::query query(domin,"http");

        boost::asio::ip::tcp::resolver::iterator endpoint_iterator = m_resolver.resolve(query);
        boost::asio::ip::tcp::resolver::iterator end;

        //����socket��������
	    boost::asio::ip::tcp::socket socket(m_ioService);
	    boost::system::error_code error = boost::asio::error::host_not_found;
	    while (error && endpoint_iterator != end)
	    {
		    socket.close();
		    socket.connect(*endpoint_iterator++, error);
	    }
	    if (error)
		    throw boost::system::system_error(error);

        // ����request
		boost::asio::streambuf request;
		std::ostream request_stream(&request);
		request_stream << "GET " << m_uri << " HTTP/1.1\r\n";
        request_stream << "host: " << domin << "\r\n";
        request_stream << "Connection:close\r\n";
        request_stream << "\r\n";

        //���Ͳ�ѯ����
        boost::asio::write(socket, request);

        //��ȡ��ѯ���
        boost::asio::streambuf response;

        //��ȡhttp���ļ�ͷ��Ȼ�󷵻�
        boost::asio::read_until(socket, response, "\r\n");

        //���http״̬��
        std::istream response_stream(&response);
		std::string http_version;
		response_stream >> http_version;
		unsigned int status_code;
		response_stream >> status_code;
		std::string status_message;
		std::getline(response_stream, status_message);
		if (!response_stream || http_version.substr(0, 5) != "HTTP/")
		{
			LOG_W("Invalid response\n");
			return false;
		}
		if (status_code != 200)
		{
			LOG_W("Response returned with status code " << status_code << "\n");
			return false;
		}
        // ������ظ������Ķ�����
    }
	catch (std::exception& e)
	{
		LOG_W("Exception: " << e.what() << "\n");
        return false;
	}
    return true;
}

bool HTTPClient::HTTPGet(std::string& requestJson, std::string& responseJson)
{
    try
    {
        boost::asio::ip::tcp::resolver::query query(m_ip, m_port);

        boost::asio::ip::tcp::resolver::iterator endpoint_iterator = m_resolver.resolve(query);
        boost::asio::ip::tcp::resolver::iterator end;

        //����socket��������
	    boost::asio::ip::tcp::socket socket(m_ioService);
	    boost::system::error_code error = boost::asio::error::host_not_found;
	    while (error && endpoint_iterator != end)
	    {
		    socket.close();
		    socket.connect(*endpoint_iterator++, error);
	    }
	    if (error)
		    throw boost::system::system_error(error);

        // ����request
		boost::asio::streambuf request;
		std::ostream request_stream(&request);
		request_stream << "POST " << m_uri << " HTTP/1.1\r\n";
        request_stream << "Content-Length: " << requestJson.length() << "\r\n";
        request_stream << "Content-Type: text/plain\r\n\r\n";
        request_stream << requestJson;

        //���Ͳ�ѯ����
        boost::asio::write(socket, request);

        //��ȡ��ѯ���
        boost::asio::streambuf response;

        //��ȡhttp���ļ�ͷ��Ȼ�󷵻�
        boost::asio::read_until(socket, response, "\r\n");

        //���http״̬��
        std::istream response_stream(&response);
		std::string http_version;
		response_stream >> http_version;
		unsigned int status_code;
		response_stream >> status_code;
		std::string status_message;
		std::getline(response_stream, status_message);
		if (!response_stream || http_version.substr(0, 5) != "HTTP/")
		{
			LOG_W("Invalid response\n");
			return false;
		}
		if (status_code != 200)
		{
			LOG_W("Response returned with status code " << status_code << "\n");
			return false;
		}

		//��ȡhttpͷ
		boost::asio::read_until(socket, response, "\r\n\r\n");

		// Process the response headers.
		std::string header;
        int contentLength = 0;
		while (std::getline(response_stream, header) && header != "\r")
        {
            if(header.find("Content-Length") != std::string::npos \
                || header.find("Content-length") != std::string::npos)
            {
                std::size_t posBegin = header.find_first_of("123456789");
                std::size_t posEnd = header.find_last_of("1234567890");
                std::string lengthStr = header.substr(posBegin, (posEnd - posBegin + 1));
                contentLength = boost::lexical_cast<int> (lengthStr);
            }

			//LOG_D(header);
        }

		// Write whatever content we already have to output.
        char buf[4*1024];
        int totalSize = 0;
		while (response.size() > 0 || boost::asio::read(socket, response, boost::asio::transfer_at_least(1), error))
        {
            int size = response.size();            
            response_stream.read(buf + totalSize, size);   
            totalSize += size;
            if(totalSize >= contentLength)
            {
                // �յ���content����
                break;
            }
        }

        buf[contentLength] = '\0';
        responseJson.append(buf);
    }
	catch (std::exception& e)
	{
		LOG_W("Exception: " << e.what() << "\n");
        return false;
	}

    return true;
}


} // http    
} // server
} // kugou
