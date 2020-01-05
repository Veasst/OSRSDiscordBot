#include "HeartbeatManager.h"

#include <chrono>

#include <boost/property_tree/json_parser.hpp>

namespace discord::websocket
{
	HeartbeatManager::HeartbeatManager(const std::shared_ptr<boost::beast::websocket::stream<boost::beast::ssl_stream<boost::asio::ip::tcp::socket>>>& websocket, const uint32_t heartbeatInterval, const std::shared_ptr<MessageListener>& messageListener)
		: heartbeatInterval(heartbeatInterval), isRunning(false), lastTime(), websocket(websocket), sequenceNumber("null")
	{
		const auto heartbeatACKOperation = 11u;
		messageListener->registerOperationHandler(heartbeatACKOperation, std::bind(&HeartbeatManager::handleHeartbeatResponse, this, std::placeholders::_1));
	}

	HeartbeatManager::~HeartbeatManager()
	{
		stop();
	}

	void HeartbeatManager::start()
	{
		if (!isRunning)
		{
			heartbeatThread = std::thread{ &HeartbeatManager::run, this };
			isRunning = true;
		}
	}

	void HeartbeatManager::stop()
	{
		if (isRunning)
		{
			isRunning = false;
			heartbeatThread.join();
		}
	}

	void HeartbeatManager::run()
	{
		while (isRunning)
		{
			auto timePassed = std::chrono::system_clock::now() - lastTime;
			if (std::chrono::duration_cast<std::chrono::milliseconds>(timePassed).count() >= heartbeatInterval)
			{
				sendHeartbeat();
				lastTime = std::chrono::system_clock::now();
			}

			Sleep(100);
		}
	}

	void HeartbeatManager::sendHeartbeat()
	{
		std::ostringstream message;
		message << "{\"op\":1,\"d\":" << sequenceNumber << "}";
		websocket->write(boost::asio::buffer(std::string("{\"op\":1,\"d\":null}")));
	}

	void HeartbeatManager::handleHeartbeatResponse(const std::string& message)
	{
		boost::property_tree::ptree ptree;
		std::istringstream is(message);
		boost::property_tree::read_json(is, ptree);

		sequenceNumber = ptree.get<std::string>("s");
	}
}
