#include "pch.h"
#include "HttpClient.h"

#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>

HttpClient::HttpClient(std::string host, std::string port) 
	: host_(host), port_(port), resolver_(ioc_), stream_(ioc_)
{
	// do nothing
}


HttpClient::~HttpClient()
{
	// Gracefully close the socket
	beast::error_code ec;
	stream_.socket().shutdown(tcp::socket::shutdown_both, ec);
}

std::string HttpClient::sendMessage(const boost::property_tree::ptree & msg)
{
	// Look up the domain name
	auto const results = resolver_.resolve(host_, port_);

	// Make the connection on the IP address we get from a lookup
	stream_.connect(results);

	http::request<http::string_body> req = request(msg);
	req.set(http::field::host, host_);

	setHeaders(msg, &req);
	setBody(msg, &req);

	// Send the HTTP request to the remote host
	http::write(stream_, req);

	// This buffer is used for reading and must be persisted
	beast::flat_buffer buffer;

	// Declare a container to hold the response
	http::response<http::string_body> res;

	// Receive the HTTP response
	http::read(stream_, buffer, res);

	return  res.body();
}

http::request<http::string_body> HttpClient::request(const boost::property_tree::ptree & pt)
{
	std::string target = pt.get_child("uri").data();
	if (pt.get_child("method").data() == "GET") {
		http::request<http::string_body> req{ http::verb::get, target, 11 };
		return req;
	}
	else if (pt.get_child("method").data() == "POST") {
		http::request<http::string_body> req{ http::verb::post, target, 11 };
		return req;
	}
	else {
		http::request<http::string_body> req;
		return req;
	}
}

void HttpClient::setHeaders(const boost::property_tree::ptree& pt, http::request<http::string_body>* req)
{
	BOOST_FOREACH(boost::property_tree::ptree::value_type const& node, pt.get_child("headers")) {
		std::string key = node.first;
		if (key == "Host") {
			// do nothing
		}
		else {
			std::string value = node.second.data();
			req->set(key, value);
		}
	}
}

void HttpClient::setBody(const boost::property_tree::ptree& pt, http::request<http::string_body>* req)
{
	std::stringstream ss;
	ss << pt.get_child("body").data();
	req->body() = ss.str();
	req->prepare_payload();
}
