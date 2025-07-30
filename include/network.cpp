//
// Created by talik on 5/30/2025.
//

#include <curl/curl.h>
#include <iostream>
#include "network.h"
#include <fstream> // For std::ofstream

Network::Network() {
	// Initialize libcurl globally. Do this once at the start of your program.
	if (const CURLcode global_init_res = curl_global_init(CURL_GLOBAL_DEFAULT); global_init_res != CURLE_OK) {
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
	}

	// Set a user agent (good practice)
	curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-c++-buraq/1.0");

	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
}

Network::~Network() {
	// Always cleanup the easy handle
	curl_easy_cleanup(curl);
	curl = nullptr;
}


// Callback for std::ofstream
size_t write_data_to_ofstream(void *ptr, size_t size, size_t nmemb, void *userdata) {
	auto *out_stream = static_cast<std::ofstream *>(userdata);
	const std::ios_base::iostate original_exceptions = out_stream->exceptions();
	out_stream->exceptions(std::ios::failbit | std::ios::badbit); // Enable for this scope

	try {
		out_stream->write(static_cast<const char*>(ptr), size * nmemb);
		out_stream->exceptions(original_exceptions); // Restore original exception mask
		return size * nmemb; // Success
	} catch (const std::ios_base::failure& e) {
		std::cerr << "Write Callback Exception: " << e.what() << std::endl;
		// Some implementations might provide an error code
		if (e.code()) { // C++11
			std::cerr << "  Error Code: " << e.code().value() << " (" << e.code().message() << ")" << std::endl;
		}
		// e.what() *might* contain text from strerror(errno) depending on the STL implementation.
		out_stream->clear(); // Clear error flags on the stream
		out_stream->exceptions(original_exceptions); // Restore original exception mask
		return 0; // Signal error to libcurl
	}
}

// libcurl write callback function
size_t Network::write_callback(void *contents, size_t size, size_t nmemb, std::string *userp) {
	userp->append((char *) contents, size * nmemb);
	return size * nmemb;
}

std::string Network::http_get(const std::string &url) const
{
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

	if (const CURLcode res = curl_easy_perform(curl); res != CURLE_OK) {
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

int Network::downloadFile(const std::string &url, const char *filename) const
{
	if (!curl) {
		// ... cleanup curl ...
		curl_global_cleanup();
		return 1;
	}

	std::ofstream output_file_stream(filename, std::ios::binary);
	if (!output_file_stream.is_open()) {
		std::cerr << "Error: Cannot open file for writing: " << filename << std::endl;
		// ... cleanup curl ...
		curl_global_cleanup();
		return 1;
	}

	// --- 4. Set libcurl options ---
	// Set the URL
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

	// Set the callback function for writing data
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data_to_ofstream);

	// Set the userdata pointer for the callback (our output file)
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &output_file_stream);

	// Follow HTTP redirects (e.g., 301, 302)
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

	// Fail verbosely if the HTTP code is >= 400
	curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);

	// --- 5. Perform the file transfer ---
	const CURLcode res = curl_easy_perform(curl);

	// --- 6. Check for errors ---
	if (res != CURLE_OK) {
		std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
		// If FAILONERROR is set, this will also trigger for HTTP 4xx/5xx errors.
	} else {
		long http_code = 0;
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
		if (http_code == 200) {
			std::cout << "Download successful!" << std::endl;
		} else {
			// This case might not be hit if FAILONERROR is effective,
			// but good for completeness or if FAILONERROR is off.
			std::cout << "Download completed with HTTP status code: " << http_code << std::endl;
		}
	}

	// --- 7. Cleanup ---
	// Close the output file
	 output_file_stream.close(); // Close after download or on error

	// Clean up the curl easy handle
	curl_easy_cleanup(curl);

	// Clean up the global libcurl environment
	// This should be called once when the program is done with libcurl.
	curl_global_cleanup();

	return (res == CURLE_OK) ? 0 : 1;
}
