//
// Created by talik on 8/15/2025.
//

#include <QMessageBox>
#include <QCoreApplication>
#include <filesystem> // Requires C++17. For older C++, use platform-specific directory iteration.
#include <fstream>

#include "../../include/buraq.h"

#include "./db_conn.h"

namespace database
{
    QVariant insertFile(const QString& filePath, const QString& title)
    {
        QSqlQuery query;
        if (!query.prepare(INSERT_FILE_SQL))
        {
            // "Error executing query:" << query.lastError().text();
        }

        query.addBindValue(filePath);
        query.addBindValue(title);
        query.exec();

        return query.lastInsertId();
    }

    QVariant deleteRow(const QString& filePath)
    {
        QSqlQuery query;
        if (!query.prepare(DELETE_BY_FILE_PATH_SQL))
        {
            // "Error executing query:" << query.lastError().text();
        }

        query.addBindValue(filePath);
        query.exec();

        return query.lastInsertId();
    }

    QList<FileObject*> findPreviouslyOpenedFiles()
    {
        QSqlQuery query;

        if (!query.exec(SELECT_FILES_SQL))
        {
            // qDebug() << "Error executing query:" << query.lastError().text();
        }

        QList<FileObject*> files;
        while (query.next())
        {
            if (const QString filePath = query.value(1).toString(); std::filesystem::exists(filePath.toStdString()))
            {
                const auto file = new FileObject;

                file->setFilePath(filePath);

                QString title = query.value(2).toString(); // column title
                file->setFileName(title);
                files.append(file);
            }
            else
            {
                // delete row with this file
                deleteRow(filePath);
            }
        }

        return files;
    }

    QSqlError init_db()
    {
        if (QSqlQuery query; !query.exec(FILES_SQL))
        {
            file_utils::file_log("Error executing query: " + query.lastError().text().toStdString());
            return query.lastError();
        }

        return {};
    }

    bool db_conn()
    {
        using file_utils::file_log;

        file_log("Initiating DB connection..");

        std::filesystem::path dirName = std::filesystem::temp_directory_path() / "Buraq" / ".data";
        if (!std::filesystem::create_directories(dirName))
        {
            file_log("Dir " + dirName.string() + " already exists.");
        }

        std::filesystem::path dbPathName = dirName / "itools.db";
        std::string dbName = dbPathName.string();

        file_log("current dir: " + std::filesystem::current_path().string());
        file_log("DB name path: " + dbName);

        try
        {
            if (std::fstream db(dbName, std::ios::in); db.is_open())
            {
                // db file exists
                db.close();
            }
            else
            {
                // create the db file
                std::ofstream outputFile(dbName, std::ios::out);
                outputFile.close();
            }

            QSqlDatabase dbEngine = QSqlDatabase::addDatabase("QSQLITE");
            dbEngine.setDatabaseName(QString::fromStdString(dbName));

            if (!dbEngine.open())
            {
                QSqlError error = dbEngine.lastError();

                QMessageBox::critical(nullptr, QObject::tr("Cannot open database"),
                                      "Unable to establish a database connection.\n"
                                      "Click Cancel to exit.",
                                      QMessageBox::Cancel);
                file_log("Failed to open DB connection.");
                file_log("DATABASE OPEN FAILED!");
                file_log("  Database file checked: " + dbName);
                file_log(&"  Error Type:"[error.type()]);
                file_log("  Error (Driver Text):" + error.driverText().toStdString());
                file_log(&"  Driver available:"[QSqlDatabase::isDriverAvailable("QSQLITE")]);
                file_log("  Error (Database Text):" + error.databaseText().toStdString());
                return false;
            }
            else
            {
                file_log("DB connection is good!"); // Initialize the database:
                if (QSqlError err = init_db(); err.type() != QSqlError::NoError)
                {
                    file_log("Error executing initializing db: " + err.text().toStdString());
                }
            }
        }
        catch (const std::ios_base::failure& failure)
        {
            file_log("failed: ");
            return false;
        }
        catch (...)
        {
            file_log("Exception catch all: db_conn failed!");
            return false;
        }

        return true;
    }
}
