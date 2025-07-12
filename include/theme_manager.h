#ifndef THEME_MANAGER_H
#define THEME_MANAGER_H

#include <QObject>
#include <QApplication>
#include <QFile>
#include <QDebug>
#include <QPalette> // Include QPalette

enum AppTheme
{
    LightTheme,
    DarkTheme
};

class ThemeManager final : public QObject
{
    Q_OBJECT

public:
    static ThemeManager& instance();

    void setAppTheme(AppTheme theme);
    AppTheme currentTheme() const { return m_currentTheme; }

protected:
    // Override the event method to catch QEvent::ApplicationPaletteChange
    bool event(QEvent *event) override; // NEW: Override event()
signals:
    void themeChanged(AppTheme theme);

private slots:
    void onApplicationPaletteChanged(const QPalette& oldPalette); // New slot

private:
    explicit ThemeManager(QObject* parent = nullptr);
    ThemeManager(const ThemeManager&) = delete;
    ThemeManager& operator=(const ThemeManager&) = delete;

    AppTheme m_currentTheme;

    // Helper to determine theme from palette
    static AppTheme getThemeFromPalette(const QPalette& palette);
};

#endif // THEME_MANAGER_H
