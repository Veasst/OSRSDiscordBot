#define CURL_STATICLIB

#include <iostream>
#include <string>

#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "DiscordAPI.h"

boost::optional<uint32_t> getHeartbeatInterval(const boost::beast::multi_buffer::const_buffers_type& data)
{
	boost::property_tree::ptree ptree;
	std::string response = boost::beast::buffers_to_string(data);
	std::istringstream is(response);
	boost::property_tree::read_json(is, ptree);

	return ptree.get_optional<uint32_t>("d.heartbeat_interval");
}

int main()
{
	std::string gatewayUrl = discord::api::getGatewayUrl();
	if (gatewayUrl.empty())
	{
		return 0;
	}

	boost::asio::io_context ioc;
	boost::asio::ip::tcp::resolver resolver{ ioc };
	boost::asio::ssl::context sslcontext{ boost::asio::ssl::context::tlsv13_client };
	boost::beast::websocket::stream<boost::beast::ssl_stream<boost::asio::ip::tcp::socket>> websocket{ ioc, sslcontext };

	try
	{
		auto const endpoints = resolver.resolve(gatewayUrl, "443");
		boost::asio::connect(websocket.next_layer().next_layer(), endpoints);
		websocket.next_layer().handshake(boost::asio::ssl::stream_base::client);
		websocket.handshake(gatewayUrl, "/?v=6&encoding=json");

		boost::beast::multi_buffer buffer;
		websocket.read(buffer);

		auto heartbeatInterval = getHeartbeatInterval(buffer.data());
		if (!heartbeatInterval)
		{
			return 0;
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what();
	}

	return 0;
}