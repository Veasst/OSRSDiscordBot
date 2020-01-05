#pragma once

#include <cstdint>

#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>

#include "MessageListener.h"

namespace discord::websocket
{
	class HeartbeatManager
	{
	public:
		HeartbeatManager(const std::shared_ptr<boost::beast::websocket::stream<boost::beast::ssl_stream<boost::asio::ip::tcp::socket>>>& websocket, const uint32_t heartbeatInterval, const std::shared_ptr<MessageListener>& messageListener);
		~HeartbeatManager();
		void start();
		void stop();
	private:
		void run();
		void sendHeartbeat();
		void handleHeartbeatResponse(const std::string& message);

		bool isRunning;
		uint32_t heartbeatInterval;
		std::chrono::system_clock::time_point lastTime;
		std::shared_ptr<boost::beast::websocket::stream<boost::beast::ssl_stream<boost::asio::ip::tcp::socket>>> websocket;
		std::shared_ptr<MessageListener> messageListener;
		std::thread heartbeatThread;
		std::string sequenceNumber;
	};
}
