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
// Created by talik on 3/9/2024.
//

#include <QGridLayout>
#include <QLabel>
#include <QFileDialog>
#include "CustomDrawer.h"
#include "IconButton.h"
#include "../db_connection.h"

namespace {

//TODO clean up for best practices
QString getFilename(const QString &filePath) {
    if(!filePath.isEmpty()) {
        long long int i = filePath.lastIndexOf("/") + 1;
        return filePath.mid(i);
    }

    return nullptr;
}
}

CustomDrawer::CustomDrawer(Editor *ptr) : editor(ptr), QWidget(ptr) {
    setStyleSheet("background-color: #252525;");

    setFixedWidth(DrawerMeasurements::width);
    setMaximumHeight(500);

    pLayout = std::make_unique<QVBoxLayout>();
    setLayout(pLayout.get());
    pLayout->setSpacing(8);
    pLayout->setContentsMargins(2, 4, 2, 4);

    auto *panel = new QWidget;
    panel->setStyleSheet(
        "color: #C2C2C2;"
        "background-color: #252525;"
    );

    auto *layout = new QGridLayout;
    panel->setLayout(layout);

    addFile = std::make_unique<IconButton>(QIcon(ItoolsNS::getConfig().getAppIcons()->addFileIcon));
    connect(addFile.get(), &IconButton::clicked, this, &CustomDrawer::onAddButtonClicked);

    auto label = new QLabel();
    label->setText("Workspace");

    layout->addWidget(addFile.get(), 0, 1, 12, 12, Qt::AlignmentFlag::AlignRight);
    layout->addWidget(label, 0, 0, 12, 1);

    pLayout->addWidget(panel, Qt::AlignmentFlag::AlignRight);

    // Read from the database and load all the files that were
    // previously opened.
    showPreviouslyOpenedFiles();

    // drawer is collapsed by default.
    // show();
}

void CustomDrawer::toggle() {
    if(isVisible()) {
        hide();
    } else {
        show();
    }
}

void CustomDrawer::onAddButtonClicked() {
    QFileDialog dialog(this);
    // Removes name filter to allow future support for other languages
    dialog.setViewMode(QFileDialog::Detail);

    QStringList fileNames;

    if(dialog.exec()) {
        fileNames = dialog.selectedFiles();
        if(!fileNames.empty()) {
            const QString &filePath = fileNames.at(0);
            const QString &fileName = getFilename(filePath);

            QVariant result = insertFile(filePath, fileName);
            if(result.isValid()) {
                createFileLabel(filePath, fileName, true);
            }
        }
    }
}

void CustomDrawer::createFileLabel(
    const QString &filePath, const QString &fileName, bool shouldAutoOpenFile) const {

    auto label = new FilePathLabel(filePath, (QWidget *) this);

    QObject::connect(label, &FilePathLabel::clicked, this, &CustomDrawer::onFileLabelClick);

    pLayout->addWidget(label);

    if(!filePath.isEmpty()) {
        openFilePath(label, filePath, fileName);

        if(shouldAutoOpenFile) {
            // updated the side panel to show this as the newly active file
            emit label->clicked();
        }
    }
}

void CustomDrawer::openFilePath(FilePathLabel *label, const QString &filePath, const QString &fileName) {
    if(!filePath.isEmpty()) {
        label->setText(fileName);

        QFile file(filePath);
        file.close();
    }
}

void CustomDrawer::onFileLabelClick() {
    QObject *senderObj = sender();
    QObject *activeItem = state.activeFileLabel;
    if(activeItem == senderObj) {
        // do nothing.
        return;
    }

    if(activeItem != nullptr) {
        auto prevActiveLabel = dynamic_cast<FilePathLabel *>(state.activeFileLabel);
        prevActiveLabel->reset();
    }

    // update active label
    auto label = dynamic_cast<FilePathLabel *>(senderObj);
    // sets active label css
    label->activeLabel();

    setActive(label);

    // Update the editor
    if(!editor) {
        return;
        return;
    }
    // TODO emit active file path
//	editor->openAndParseFile(label->getFilePath(), QFile::OpenModeFlag::ReadWrite);
}

void CustomDrawer::setActive(QWidget *pLabel) {
    state.activeFileLabel = pLabel;
}

void CustomDrawer::showPreviouslyOpenedFiles() {
    auto previousOpenedFiles = findPreviouslyOpenedFiles();

    bool isFirstTime = true;
    for(auto file : previousOpenedFiles) {
        try {
            createFileLabel(file->getFilePath(), file->getFileName(), isFirstTime);

            if(isFirstTime) {
                isFirstTime = false;
            }
        } catch(...) {
            deleteRow(file->getFilePath());
        }
    }
}
