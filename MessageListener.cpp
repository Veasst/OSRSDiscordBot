#include "MessageListener.h"

#include <boost/property_tree/json_parser.hpp>

namespace discord::websocket
{
	MessageListener::MessageListener(const std::shared_ptr<boost::beast::websocket::stream<boost::beast::ssl_stream<boost::asio::ip::tcp::socket>>>& websocket)
		: websocket(websocket)
	{
	}

	void MessageListener::run()
	{
		boost::property_tree::ptree ptree;
		while (1)
		{
			boost::beast::multi_buffer buffer;
			websocket->read(buffer);

			std::string response = boost::beast::buffers_to_string(buffer.data());
			std::istringstream is(response);
			boost::property_tree::read_json(is, ptree);

			auto eventName = ptree.get<std::string>("t");
			if (eventHandlers.find(eventName) != eventHandlers.end())
			{
				for (const auto& handler : eventHandlers[eventName])
				{
					handler(response);
				}
			}

			auto operationType = ptree.get<uint32_t>("op");
			if (operationHandlers.find(operationType) != operationHandlers.end())
			{
				for (const auto& handler : operationHandlers[operationType])
				{
					handler(response);
				}
			}
		}
	}

	void MessageListener::registerEventHandler(const std::string eventName, const std::function<void(const std::string & message)>& callback)
	{
		eventHandlers[eventName].push_back(callback);
	}

	void MessageListener::registerOperationHandler(const uint32_t operationId, const std::function<void(const std::string & message)>& callback)
	{
		operationHandlers[operationId].push_back(callback);
	}
}
