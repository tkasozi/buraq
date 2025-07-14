//
// Created by talik on 3/12/2024.
//

#ifndef IT_TOOLS_FILEPATHLABEL_H
#define IT_TOOLS_FILEPATHLABEL_H

#include <QLabel>
#include <QMouseEvent>
#include <utility>
#include "CustomDrawer.h"
#include "CustomLabel.h"

class FilePathLabel final : public CustomLabel
{
    Q_OBJECT

public slots:
    void activeLabel()
    {
        isActive = true;
    }

    void reset()
    {
        isActive = false;
    }

public:
    explicit FilePathLabel(QString filePath, QWidget* parent = nullptr)
        : CustomLabel(parent), isActive(false), filePath(std::move(filePath))
    {
        setMouseTracking(true);
    }

    ~FilePathLabel() override = default;

    QString getFilePath()
    {
        return filePath;
    };

private:
    bool isActive;
    QString filePath;
};

#endif //IT_TOOLS_FILEPATHLABEL_H
