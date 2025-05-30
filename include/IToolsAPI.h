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
// Created by talik on 5/16/2025.
//

#ifndef ITOOLS_API_H
#define ITOOLS_API_H

#include <filesystem>
#include <string>
#include <set>
#include <map>

// Example of an application context you might pass to plugins
struct IToolsApi {
	std::filesystem::path searchPath;
	std::map<std::string, std::string> plugins;
};

struct EditorState {

	bool hasText;
	bool isBlockValid;
	bool isSelected;
	int blockCount;
	int cursorBlockNumber;
	int blockNumber;
	int lineHeight;
	int currentLineHeight;
	std::set<int> selectedBlockNumbers;

	// For the updateEditorState check if states are different
	bool operator!=(const EditorState &other) const {
		return blockCount != other.blockCount ||
			   blockNumber != other.blockNumber ||
			   cursorBlockNumber != other.cursorBlockNumber;
	}

	bool operator==(const EditorState &other) const {
		return !(*this != other);
	}
};

#endif //ITOOLS_API_H
