#pragma once
// BOOST BEAST
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
// BOOST PT
#include <boost/property_tree/ptree.hpp>
#include <string>

namespace beast = boost::beast;     // from <boost/beast.hpp>
namespace http = beast::http;       // from <boost/beast/http.hpp>
namespace net = boost::asio;        // from <boost/asio.hpp>
using tcp = net::ip::tcp;           // from <boost/asio/ip/tcp.hpp>

class HttpClient
{
public:
	HttpClient(std::string host, std::string port);
	virtual ~HttpClient();
	std::string sendMessage(const boost::property_tree::ptree& msg);

private:
	http::request<http::string_body> request(const boost::property_tree::ptree& pt);
	void setHeaders(const boost::property_tree::ptree& pt, http::request<http::string_body>* req);
	void setBody(const boost::property_tree::ptree& pt, http::request<http::string_body>* req);
	

private:
	std::string host_;
	std::string port_;

	// required for all beast I/O
	net::io_context ioc_;
	tcp::resolver resolver_;
	beast::tcp_stream stream_;
};

