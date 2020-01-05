#pragma once

#include <functional>
#include <map>
#include <string>
#include <vector>

#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>

namespace discord::websocket
{
	class MessageListener
	{
	public:
		MessageListener(const std::shared_ptr<boost::beast::websocket::stream<boost::beast::ssl_stream<boost::asio::ip::tcp::socket>>>& websocket);
		void registerEventHandler(const std::string eventName, const std::function<void(const std::string& message)>& callback);
		void registerOperationHandler(const uint32_t operationId, const std::function<void(const std::string& message)>& callback);
		void run();

	private:
		std::map<std::string, std::vector<std::function<void(const std::string & message)>>> eventHandlers;
		std::map<uint32_t, std::vector<std::function<void(const std::string & message)>>> operationHandlers;
		std::shared_ptr<boost::beast::websocket::stream<boost::beast::ssl_stream<boost::asio::ip::tcp::socket>>> websocket;
	};
}
