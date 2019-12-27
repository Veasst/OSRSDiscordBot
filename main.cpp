#include <iostream>
#include <string>

#include "DiscordWebSocket.h"

int main()
{
	try
	{
		discord::websocket::DiscordWebSocket discordWebSocket{};
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what();
	}

	return 0;
}
