#pragma once

#include <memory>

#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>

namespace discord::websocket
{
	class DiscordWebSocket
	{
	public:
		DiscordWebSocket();
		~DiscordWebSocket();

	private:
		boost::optional<uint32_t> getHeartbeatInterval(const boost::beast::multi_buffer::const_buffers_type& data);

		std::unique_ptr<boost::beast::websocket::stream<boost::beast::ssl_stream<boost::asio::ip::tcp::socket>>> websocket;
	};
}
