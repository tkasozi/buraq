//
// Created by talik on 8/30/2025.
//

#include "settingsdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTabWidget>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QLabel>
#include <QFormLayout>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QGroupBox>

#include "Config.h"

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Settings");

    // --- Main Tab Widget ---
    m_tabWidget = new QTabWidget(this);
    m_tabWidget->addTab(createAppearancePage(), "Appearance");
    m_tabWidget->addTab(createEditorPage(), "Editor");
    m_tabWidget->addTab(createAccountPage(), "Account");

    // --- Bottom Button Box (OK, Cancel, Apply) ---
    m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Apply, this);
    connect(m_buttonBox, &QDialogButtonBox::accepted, this, &SettingsDialog::accept);
    connect(m_buttonBox, &QDialogButtonBox::rejected, this, &SettingsDialog::reject);
    connect(m_buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked, this, &SettingsDialog::applyChanges);

    // --- Main Layout ---
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(m_tabWidget);
    mainLayout->addWidget(m_buttonBox);
    setLayout(mainLayout);

    const auto windowConfig = Config::singleton().getWindow();
    // Set an initial size
    this->resize(windowConfig->normalSize, windowConfig->minHeight);
}

SettingsDialog::~SettingsDialog() = default;

void SettingsDialog::applyChanges()
{
    // In a real application, you would save the settings here.
    // For example, using QSettings.
    qDebug() << "Apply button clicked. Settings would be saved here.";

    // Example of how you might retrieve a value:
    // QComboBox* themeCombo = findChild<QComboBox*>("themeComboBox");
    // if(themeCombo) {
    //     QString selectedTheme = themeCombo->currentText();
    //     QSettings settings;
    //     settings.setValue("theme", selectedTheme);
    // }
}

QWidget* SettingsDialog::createAppearancePage()
{
    QWidget *pageWidget = new QWidget(this);
    QFormLayout *layout = new QFormLayout(pageWidget);

    // --- Theme Selection ---
    QGroupBox *themeGroup = new QGroupBox("Theme", this);
    QFormLayout *themeLayout = new QFormLayout(themeGroup);

    QComboBox *themeComboBox = new QComboBox(this);
    themeComboBox->setObjectName("themeComboBox"); // For retrieving the value later
    themeComboBox->addItems({"Light", "Dark", "System Default"});
    themeLayout->addRow(new QLabel("Application Theme:", this), themeComboBox);

    layout->addWidget(themeGroup);

    // --- Font Selection ---
    QGroupBox *fontGroup = new QGroupBox("Typography", this);
    QFormLayout *fontLayout = new QFormLayout(fontGroup);

    QComboBox *fontComboBox = new QComboBox(this);
    fontComboBox->addItems({"Segoe UI", "Roboto", "Helvetica"});
    QSpinBox *fontSizeSpinBox = new QSpinBox(this);
    fontSizeSpinBox->setRange(8, 20);
    fontSizeSpinBox->setValue(10);
    fontSizeSpinBox->setSuffix(" pt");

    fontLayout->addRow(new QLabel("Font:", this), fontComboBox);
    fontLayout->addRow(new QLabel("Font Size:", this), fontSizeSpinBox);

    layout->addWidget(fontGroup);

    pageWidget->setLayout(layout);
    return pageWidget;
}

QWidget* SettingsDialog::createEditorPage()
{
    QWidget *pageWidget = new QWidget(this);
    QFormLayout *layout = new QFormLayout(pageWidget);
    layout->setSpacing(15);

    QSpinBox *tabSizeSpinBox = new QSpinBox(this);
    tabSizeSpinBox->setValue(4);

    QCheckBox *wordWrapCheckBox = new QCheckBox("Enable word wrap", this);
    wordWrapCheckBox->setChecked(true);

    QCheckBox *autoIndentCheckBox = new QCheckBox("Enable auto-indentation", this);
    autoIndentCheckBox->setChecked(true);

    QCheckBox *showLineNumbersCheckBox = new QCheckBox("Show line numbers", this);
    showLineNumbersCheckBox->setChecked(true);

    layout->addRow("Tab Size:", tabSizeSpinBox);
    layout->addRow(wordWrapCheckBox);
    layout->addRow(autoIndentCheckBox);
    layout->addRow(showLineNumbersCheckBox);

    pageWidget->setLayout(layout);
    return pageWidget;
}

QWidget* SettingsDialog::createAccountPage()
{
    QWidget *pageWidget = new QWidget(this);
    QFormLayout *layout = new QFormLayout(pageWidget);

    QLineEdit *apiKeyLineEdit = new QLineEdit(this);
    apiKeyLineEdit->setPlaceholderText("Enter your API key");
    apiKeyLineEdit->setEchoMode(QLineEdit::Password);

    QPushButton *loginButton = new QPushButton("Connect Account", this);

    layout->addRow("API Key:", apiKeyLineEdit);
    layout->addRow("", loginButton);

    pageWidget->setLayout(layout);
    return pageWidget;
}
