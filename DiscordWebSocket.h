#pragma once

#include <memory>

#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>

#include "HeartbeatManager.h"
#include "MessageListener.h"

namespace discord::websocket
{
	class DiscordWebSocket
	{
	public:
		DiscordWebSocket(const std::string& discordToken);

	private:
		boost::optional<uint32_t> getHeartbeatInterval(const boost::beast::multi_buffer::const_buffers_type& data);

		boost::asio::io_context ioc;
		std::shared_ptr<boost::beast::websocket::stream<boost::beast::ssl_stream<boost::asio::ip::tcp::socket>>> websocket;
		std::shared_ptr<MessageListener> messageListener;
		std::unique_ptr<HeartbeatManager> heartbeatManager;
	};
}
