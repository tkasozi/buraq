#include "theme_manager.h"

ThemeManager::ThemeManager(QObject* parent)
    : QObject(parent), m_currentTheme(DarkTheme) // Default to DarkTheme
{
    // Install THIS ThemeManager instance as an event filter on the QApplication object.
    // This allows its eventFilter() method to intercept events sent to qApp.
    qApp->installEventFilter(this); // CRUCIAL: Install self as filter on qApp

    // Set initial theme based on system or preference
    setAppTheme(m_currentTheme); // Load initial theme stylesheet
}

ThemeManager& ThemeManager::instance()
{
    static ThemeManager manager;
    return manager;
}

void ThemeManager::setAppTheme(AppTheme theme)
{
    // Important: Only apply stylesheet if it's a different theme
    if (m_currentTheme == theme && !qApp->styleSheet().isEmpty())
    {
        return;
    }

    QString stylePath;
    switch (theme)
    {
    case DarkTheme:
        stylePath = ":/styles/dark_theme.qss";
        break;
    case LightTheme:
        stylePath = ":/styles/light_theme.qss";
        break;
    }

    if (QFile styleFile(stylePath); styleFile.open(QFile::ReadOnly | QFile::Text))
    {
        const QString style = styleFile.readAll();
        qApp->setStyleSheet(style);
        styleFile.close();
        m_currentTheme = theme;
        qDebug() << "Application theme set to:" << (theme == DarkTheme ? "Dark" : "Light");
        emit themeChanged(m_currentTheme);
    }
    else
    {
        qWarning() << "Failed to load stylesheet:" << stylePath;
    }
}

bool ThemeManager::eventFilter(QObject* watched, QEvent* event)
{
    // Check if the event is for the QApplication object itself
    if (watched == qApp && event->type() == QEvent::ApplicationPaletteChange)
    {
        qDebug() << "Event filter caught QEvent::ApplicationPaletteChange!";

        if (const AppTheme detectedTheme = getThemeFromPalette(qApp->palette()); detectedTheme != m_currentTheme)
        {
            qDebug() << "System palette changed. Detected new theme:" << (
                detectedTheme == DarkTheme ? "Dark" : "Light");
            setAppTheme(detectedTheme); // Apply the detected theme
        }
        return true; // Event handled, stop propagation
    }
    // For other events or if not watching qApp, pass to base class eventFilter
    return QObject::eventFilter(watched, event);
}

// Helper function to determine theme from palette (remains the same)
AppTheme ThemeManager::getThemeFromPalette(const QPalette& palette)
{
    const QColor windowColor = palette.color(QPalette::Window);
    const double luminance = (0.2126 * windowColor.red() +
        0.7152 * windowColor.green() +
        0.0722 * windowColor.blue()) / 255.0;

    if (constexpr double DARK_THEME_LUMINANCE_THRESHOLD = 0.3; luminance < DARK_THEME_LUMINANCE_THRESHOLD)
    {
        return DarkTheme;
    }
    return LightTheme;
}
