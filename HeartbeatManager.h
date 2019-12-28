#pragma once

#include <cstdint>

#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>

namespace discord::websocket
{
	class HeartbeatManager
	{
	public:
		HeartbeatManager(const std::shared_ptr<boost::beast::websocket::stream<boost::beast::ssl_stream<boost::asio::ip::tcp::socket>>>& websocket, const uint32_t heartbeatInterval);
		~HeartbeatManager();
		void start();
		void stop();
	private:
		void run();
		void sendHeartbeat();

		bool isRunning;
		uint32_t heartbeatInterval;
		std::chrono::system_clock::time_point lastTime;
		std::shared_ptr<boost::beast::websocket::stream<boost::beast::ssl_stream<boost::asio::ip::tcp::socket>>> websocket;
		std::thread heartbeatThread;
	};
}
