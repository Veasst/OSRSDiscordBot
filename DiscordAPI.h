#pragma once

#include <string>

#include <boost/property_tree/json_parser.hpp>

#include <curl/curl.h>

namespace discord::api
{

	size_t writeResponse(const char* source, const size_t size, const size_t numberOfElements, void* destination)
	{
		((std::string*)destination)->append((char*)source, size * numberOfElements);
		return size * numberOfElements;
	}

	std::string getGatewayUrl()
	{
		CURL* curl;
		std::string response;

		curl = curl_easy_init();
		curl_easy_setopt(curl, CURLOPT_URL, "https://discordapp.com/api/gateway");
		curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeResponse);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

		curl_easy_perform(curl);

		long respcode;
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &respcode);
		curl_easy_cleanup(curl);

		if (respcode != 200)
		{
			return "";
		}

		boost::property_tree::ptree ptree;
		std::istringstream is(response);
		boost::property_tree::read_json(is, ptree);

		auto gatewayUrl = ptree.get_optional<std::string>("url");
		if (!gatewayUrl)
		{
			return "";
		}

		return (*gatewayUrl).substr(6, (*gatewayUrl).size() - 6);
	}
}