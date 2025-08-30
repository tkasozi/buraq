//
// Created by talik on 8/30/2025.
//

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

// Forward declarations for Qt classes to speed up compilation
class QTabWidget;
class QDialogButtonBox;
class QListWidget;
class QStackedWidget;

class SettingsDialog final : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog() override;

private slots:
    // Slot to handle saving the settings
    void applyChanges();

private:
    // Helper functions to create each page of the settings dialog
    QWidget* createAppearancePage();
    QWidget* createEditorPage();
    QWidget* createAccountPage();

    // Main UI elements
    QTabWidget* m_tabWidget;
    QDialogButtonBox* m_buttonBox;
};

#endif // SETTINGSDIALOG_H
