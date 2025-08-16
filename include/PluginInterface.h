// MIT License
//
// Copyright (c)  "2025" Talik A. Kasozi
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

//
// Created by talik on 5/15/2025.
//

#ifndef PLUGIN_INTERFACE_H
#define PLUGIN_INTERFACE_H

#include <string>
#include "buraq.h"

struct ProcessedData {
	std::wstring resultValue;
};

class IPlugin {
public:

	explicit IPlugin(buraq::buraq_api *api_context) : api_context(api_context){};

	virtual ~IPlugin() = default;

	virtual const char* getName() const = 0;
	virtual void shutdown() = 0;
	virtual ProcessedData performAction(void *cmd) = 0;

	/**
	 *
	 * @param app_context A pointer to an object or struct in the main thread that provides necessary data to
	 * the plugin.
	 * @return True if successfully initialized.
	 */
	virtual bool initialize(buraq::buraq_api* app_context) = 0;
private:
	buraq::buraq_api *api_context;
};

// Avoid C++ name mangling
extern "C" {
typedef  IPlugin* (*CreatePluginFunc)(buraq::buraq_api *);
typedef void (*DestroyPluginFunc)(IPlugin*);
}

#endif //PLUGIN_INTERFACE_H