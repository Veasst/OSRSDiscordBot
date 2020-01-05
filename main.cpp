#include <fstream>
#include <iostream>
#include <string>

#include <boost/property_tree/json_parser.hpp>

#include "DiscordWebSocket.h"

int main()
{
	std::ifstream configFile{"config.json"};
	if (!configFile.is_open())
	{
		std::cerr << "Failed to open config file";
		return 0;
	}

	std::stringstream config;
	config << configFile.rdbuf();
	boost::property_tree::ptree ptree;
	boost::property_tree::read_json(config, ptree);

	const auto discordToken = ptree.get_optional<std::string>("token");
	if (!discordToken)
	{
		std::cerr << "Failed to find Discord token";
		return 0;
	}
	try
	{
		discord::websocket::DiscordWebSocket discordWebSocket{*discordToken};
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what();
	}

	return 0;
}
