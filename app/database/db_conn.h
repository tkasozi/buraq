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

#include <QSqlError>
#include <QSqlQuery>

#include "../FileObject.h"

namespace database
{
    constexpr auto FILES_SQL = "CREATE TABLE IF NOT EXISTS files(id INTEGER PRIMARY KEY, file_path VARCHAR UNIQUE, file_name VARCHAR);";

    constexpr auto INSERT_FILE_SQL = "INSERT INTO files(file_path, file_name) VALUES(?, ?);";

    constexpr auto SELECT_FILES_SQL = "SELECT * FROM files;";

    constexpr auto SELECT_FILE_BY_FILE_PATH_SQL = "SELECT * FROM files WHERE file_path = ?;";

    constexpr auto DELETE_BY_FILE_PATH_SQL ="DELETE FROM files WHERE file_path = ?;";

    QVariant insertFile(const QString& filePath, const QString& title);
    QVariant deleteRow(const QString& filePath);
    QList<FileObject*> findPreviouslyOpenedFiles();
    QSqlError init_db();
    bool db_conn();
}

#endif // IT_TOOLS_DB_CONN_H
