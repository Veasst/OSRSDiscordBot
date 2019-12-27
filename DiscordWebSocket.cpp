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

		boost::asio::io_context ioc;
		boost::asio::ip::tcp::resolver resolver{ ioc };
		boost::asio::ssl::context sslcontext{ boost::asio::ssl::context::tlsv13_client };

		websocket = std::make_unique<boost::beast::websocket::stream<boost::beast::ssl_stream<boost::asio::ip::tcp::socket>>>(ioc, sslcontext);

		try
		{
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
		}
		catch (const std::exception&)
		{
			websocket.release();
			throw;
		}
	}

	DiscordWebSocket::~DiscordWebSocket()
	{
		if (websocket)
		{
			websocket.release();
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
