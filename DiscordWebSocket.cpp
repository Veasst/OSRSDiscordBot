#include "DiscordWebSocket.h"

#include <boost/property_tree/json_parser.hpp>

#include "DiscordAPI.h"

namespace discord::websocket
{
	DiscordWebSocket::DiscordWebSocket()
	{
		std::string gatewayUrl = discord::api::getGatewayUrl();
		if (gatewayUrl.empty())
		{
			throw std::runtime_error{ "Couldn't fetch gatewayUrl" };
		}

		boost::asio::ssl::context sslcontext{ boost::asio::ssl::context::tlsv13_client };
		websocket = std::make_shared<boost::beast::websocket::stream<boost::beast::ssl_stream<boost::asio::ip::tcp::socket>>>(ioc, sslcontext);

		try
		{
			boost::asio::ip::tcp::resolver resolver{ ioc };
			auto const endpoints = resolver.resolve(gatewayUrl, "443");
			boost::asio::connect(websocket->next_layer().next_layer(), endpoints);
			websocket->next_layer().handshake(boost::asio::ssl::stream_base::client);
			websocket->handshake(gatewayUrl, "/?v=6&encoding=json");

			boost::beast::multi_buffer buffer;
			websocket->read(buffer);

			auto heartbeatInterval = getHeartbeatInterval(buffer.data());
			if (!heartbeatInterval)
			{
				throw std::runtime_error{ "Couldn't fetch heartbeatInterval" };
			}

			heartbeatManager = std::make_unique<HeartbeatManager>(websocket, *heartbeatInterval);
			heartbeatManager->start();
		}
		catch (const std::exception&)
		{
			throw;
		}
	}

	boost::optional<uint32_t> DiscordWebSocket::getHeartbeatInterval(const boost::beast::multi_buffer::const_buffers_type& data)
	{
		boost::property_tree::ptree ptree;
		std::string response = boost::beast::buffers_to_string(data);
		std::istringstream is(response);
		boost::property_tree::read_json(is, ptree);

		return ptree.get_optional<uint32_t>("d.heartbeat_interval");
	}
}
