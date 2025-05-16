//
// Created by talik on 4/26/2024.
//

#ifndef IT_TOOLS_FILEOBJECT_H
#define IT_TOOLS_FILEOBJECT_H

#include <QString>

class FileObject {

public:
	FileObject() = default;

	~FileObject();

	void setFileName(const QString &fName);

	void setFilePath(const QString &fPath);

	QString getFileName();

	QString getFilePath();

private:
	QString fileName;
	QString filePath;
};

#endif //IT_TOOLS_FILEOBJECT_H
