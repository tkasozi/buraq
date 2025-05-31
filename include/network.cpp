//
// Created by talik on 5/30/2025.
//

#include <curl/curl.h>
#include <iostream>
#include "network.h"

Network::Network() {
	// Initialize libcurl globally. Do this once at the start of your program.
	CURLcode global_init_res = curl_global_init(CURL_GLOBAL_DEFAULT);
	if (global_init_res != CURLE_OK) {
		// This is a serious issue, often best to terminate or throw.
		// Note: curl_global_init is not perfectly thread-safe if called concurrently
		// by different parts of a very complex application before the singleton is first accessed.
		// However, for typical singleton usage where it's initialized early, it's usually fine.
		throw std::runtime_error(std::string("curl_global_init() failed: ") + curl_easy_strerror(global_init_res));
	}

	curl = curl_easy_init(); // <--- Initialize the easy handle
	if (!curl) {
		// Handle error: libcurl easy handle could not be created
		throw std::runtime_error("curl_easy_init() failed. CURL easy handle could not be created.");
		exit(-1);
	}
}

Network::~Network() {
	// Always cleanup the easy handle
	curl_easy_cleanup(curl);
	curl = nullptr;
}

// libcurl write callback function
size_t Network::write_callback(void *contents, size_t size, size_t nmemb, std::string *userp) {
	userp->append((char *) contents, size * nmemb);
	return size * nmemb;
}

std::string Network::http_get(const std::string &url) {
	if (!curl) {
		// This shouldn't happen if constructor succeeded, and it's not cleaned up elsewhere
		return "Error: CURL handle not initialized.";
	}

	std::string readBuffer;

	// Set URL
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	// Set the write callback function
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
	// Pass the readBuffer string to the callback
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

	// You might want to reset other options if the handle is reused extensively
	curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L); // Ensure it's a GET

	CURLcode res = curl_easy_perform(curl);

	if (res != CURLE_OK) {
		std::cerr << "curl_easy_perform() failed for URL " << url << ": "
				  << curl_easy_strerror(res) << std::endl;
		throw std::runtime_error("Status not OK!");
	}

	long http_code = 0;
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
	if (http_code != 200) {
		std::cerr << "HTTP GET request to " << url << " returned HTTP code " << http_code << std::endl;
		std::cerr << "Response body: " << readBuffer << std::endl; // Log server error message
		// Handle non-200 responses appropriately
	}
	// For a reusable handle, consider what to do if a request fails.
	// Sometimes, specific errors might require re-initializing the handle.

	return readBuffer;
}

// network.cpp
Network &Network::singleton() {
	static Network instance; // Created once, thread-safe since C++11
	return instance;
}

void Network::http_post(const std::string &url) {

}

void Network::http_put(const std::string &url) {

}

void Network::http_delete(const std::string &url) {

}
