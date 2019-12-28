#include "HeartbeatManager.h"

#include <chrono>
#include <iostream>

namespace discord::websocket
{
	HeartbeatManager::HeartbeatManager(const std::shared_ptr<boost::beast::websocket::stream<boost::beast::ssl_stream<boost::asio::ip::tcp::socket>>>& websocket, const uint32_t heartbeatInterval)
		: isRunning(false), lastTime(), websocket(websocket), heartbeatInterval(heartbeatInterval)
	{
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
		boost::beast::multi_buffer buffer;
		websocket->write(boost::asio::buffer(std::string{ "{\"op\":1,\"d\":null}" }));
		websocket->read(buffer);
	}
}
