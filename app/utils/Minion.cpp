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
// Created by talik on 4/30/2024.
//

#include "Minion.h"
#include <QVariant>


Minion::Minion(QObject *parent) : QObject(parent) {
	// empty
}

void Minion::doWork(const std::function<QVariant()>& task) {
	if (!task) {
		emit resultReady(QVariant());
		emit workFinished();
		return;
	}

	QVariant result;

	try {
		// Execute the provided task function
		emit progressUpdated(0);
		result = task(); // Execute the generic task
	} catch (const std::exception &e) {
		// Optionally, wrap the error message in the QVariant
		result = QVariant("Error: " + QString(e.what()));
	} catch (...) {
		// Optionally, wrap the error message in the QVariant
		result = QVariant("Error: Unknown exception");
	}
	emit resultReady(result);
	emit workFinished();
}
