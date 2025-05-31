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
// Created by talik on 4/26/2024.
//

#ifndef IT_TOOLS_DB_CONN_H
#define IT_TOOLS_DB_CONN_H

#include <QSqlDatabase>
#include <QMessageBox>
#include <QSqlError>
#include <QDate>
#include <QSqlQuery>
#include "FileObject.h"
#include <fstream>
#include <QStandardPaths> // For user-writable locations
#include <QCoreApplication>
#include <filesystem> // Requires C++17. For older C++, use platform-specific directory iteration.


#include <chrono>
#include <iomanip> // For std::put_time
#include <ctime>   // For std::localtime, std::time_t


const auto FILES_SQL = QLatin1String(R"(
    CREATE TABLE IF NOT EXISTS files(id INTEGER PRIMARY KEY, file_path VARCHAR UNIQUE, file_name VARCHAR);
)");

const auto INSERT_FILE_SQL = QLatin1String(R"(
    INSERT INTO files(file_path, file_name) VALUES(?, ?);
)");

const auto SELECT_FILES_SQL = QLatin1String(R"(
    SELECT * FROM files;
)");

const auto SELECT_FILE_BY_FILE_PATH_SQL = QLatin1String(R"(SELECT * FROM files WHERE file_path = ?;)");

static QVariant insertFile(const QString &filePath, const QString &title) {
	QSqlQuery query;
	if (!query.prepare(INSERT_FILE_SQL)) {
		// "Error executing query:" << query.lastError().text();
	}

	query.addBindValue(filePath);
	query.addBindValue(title);
	query.exec();

	return query.lastInsertId();
}

static QList<FileObject *> findPreviouslyOpenedFiles() {
	QSqlQuery query;

	if (!query.exec(SELECT_FILES_SQL)) {
		// qDebug() << "Error executing query:" << query.lastError().text();
	}

	QList<FileObject *> files;
	while (query.next()) {
		auto file = new FileObject;

		int id = query.value(0).toInt();
		QString filePath = query.value(1).toString(); // column file_path
		QString title = query.value(2).toString();      // column title

		file->setFilePath(filePath);
		file->setFileName(title);
		files.append(file);
	}

	return files;
}

// Function to log messages (example)
static void db_log(const QString &message) {
	// Optionally, log to a file in a writable location:
	std::filesystem::path logFilePath = std::filesystem::temp_directory_path() / "ITools" / ".data" / "log.txt";
	QFile logFile(logFilePath);
	try {
		std::ofstream outputFile(logFilePath, std::ios::out);
		outputFile.exceptions(std::ifstream::failbit);

		// 1. Get the current time point from the system_clock
		std::chrono::system_clock::time_point now_tp = std::chrono::system_clock::now();

		// 2. Convert the time_point to a time_t (needed for some C-style functions like std::localtime)
		std::time_t now_c = std::chrono::system_clock::to_time_t(now_tp);

		// 3. Convert to a struct tm for more detailed fields (local time)
		//    std::localtime is not thread-safe on all platforms without _s versions.
		std::tm local_tm = *std::localtime(&now_c); // Dereference pointer returned by localtime

		// Print the date and time using std::put_time (C++11)
		outputFile << std::put_time(&local_tm, "%Y-%m-%d %H:%M:%S")  << ": " << message << std::endl;
		outputFile.close();
	} catch (const std::ios_base::failure &failure) {
		qDebug() << "Fails" ;
	}
}

static bool db_conn() {
	db_log("Initiating DB connection..");

	std::filesystem::path dirName = std::filesystem::temp_directory_path() / "ITools" / ".data";
	if (!std::filesystem::create_directories(dirName)) {
		db_log("Dir " + QString::fromStdString(dirName.string()) + " already exists.");
	}

	std::filesystem::path dbPathName = dirName / "itools.db";
	std::string dbName = dbPathName.string();

	db_log("current dir: " + QString::fromStdString(std::filesystem::current_path().string()));
	db_log("DB name path: " + QString::fromStdString(dbName));

	try {
		std::fstream db(dbName, std::ios::in);

		if (db.is_open()) {
			// db file exists
			db.close();
		} else {
			std::ofstream outputFile(dbName, std::ios::out);
			outputFile.close();
		}

		QSqlDatabase dbEngine = QSqlDatabase::addDatabase("QSQLITE");
		dbEngine.setDatabaseName(QString::fromStdString(dbName));

		if (!dbEngine.open()) {
			QSqlError error = dbEngine.lastError();

			QMessageBox::critical(nullptr, QObject::tr("Cannot open database"),
								  "Unable to establish a database connection.\n"
								  "Click Cancel to exit.",
								  QMessageBox::Cancel);
			db_log("Failed to open DB connection.");
			db_log("DATABASE OPEN FAILED!");
			db_log("  Database file checked: " + QString::fromStdString(dbName));
			db_log(&"  Error Type:" [error.type()]);
			db_log("  Error (Driver Text):" + error.driverText());
			db_log(&"  Driver available:" [QSqlDatabase::isDriverAvailable("QSQLITE")]);
			db_log("  Error (Database Text):" + error.databaseText());
			return false;
		} else {
			db_log("DB connection is good!");
		}
	}
	catch (_exception e) {
		db_log(QString(e.name) + QString("Exception in try-catch"));
		return false;
	}
	catch (...) {
		db_log("Exception catch all: db_conn failed!");
		return false;
	}

	return true;
}

static QSqlError init_db() {

	QSqlQuery query;
	if (!query.exec(FILES_SQL)) {
		db_log("Error executing query: " + query.lastError().text());
		return query.lastError();
	}

	return {};
}

#endif // IT_TOOLS_DB_CONN_H
