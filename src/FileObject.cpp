//
// Created by talik on 4/26/2024.
//
#include "FileObject.h"

FileObject::~FileObject() = default;

QString FileObject::getFileName() {
	return fileName;
}

QString FileObject::getFilePath() {
	return filePath;
}

void FileObject::setFileName(const QString &fName) {
	this->fileName = fName;
}

void FileObject::setFilePath(const QString &fPath) {
	this->filePath = fPath;
}
