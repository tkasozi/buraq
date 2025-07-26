//
// Created by talik on 5/30/2025.
//

#ifndef NETWORK_H
#define NETWORK_H

#include <curl/curl.h>
#include <string>

class Network {
public:
	static Network &singleton(); // Return a reference
	std::string http_get(const std::string &url) const;

	static size_t write_callback(void *contents, size_t size, size_t nmemb, std::string *userp);

	int downloadFile(const std::string &url, const char *filename) const;

	[[maybe_unused]] [[maybe_unused]] void http_post(const std::string &url);

	[[maybe_unused]] [[maybe_unused]] void http_put(const std::string &url);

	[[maybe_unused]] [[maybe_unused]] void http_delete(const std::string &url);

private:
	Network();

	~Network();

	Network(const Network &) = delete; // No copy constructor
	Network &operator=(const Network &) = delete; // No copy assignment

	CURL *curl;
};

#endif //NETWORK_H
