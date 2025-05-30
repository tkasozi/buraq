//
// Created by talik on 5/30/2025.
//

#ifndef NETWORK_H
#define NETWORK_H

#include <string>

void http_get(const std::string& url);

[[maybe_unused]] void http_post(std::string url);

[[maybe_unused]] void http_put(std::string url);

[[maybe_unused]] void http_delete(std::string url);

#endif //NETWORK_H
