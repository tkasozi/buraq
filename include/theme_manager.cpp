#include "theme_manager.h"

ThemeManager::ThemeManager(QObject *parent)
    : QObject(parent), m_currentTheme(DarkTheme) // Default to DarkTheme
{
    // Set initial theme based on system or preference
    // You might want to call setAppTheme(getThemeFromPalette(qApp->palette()));
    // or set a default and let the first paletteChanged signal adjust it.
    // or set basing on user settings
    setAppTheme(m_currentTheme); // Load initial theme stylesheet
}

ThemeManager& ThemeManager::instance()
{
    static ThemeManager manager;
    return manager;
}

void ThemeManager::setAppTheme(const AppTheme theme)
{
    // Important: Only apply stylesheet if it's a different theme
    // This prevents infinite loops if onApplicationPaletteChanged calls setAppTheme
    if (m_currentTheme == theme && !qApp->styleSheet().isEmpty()) {
        return;
    }

    QString stylePath;
    switch (theme) {
        case DarkTheme:
            stylePath = ":/styles/dark_theme.qss";
            break;
        case LightTheme:
            stylePath = ":/styles/light_theme.qss";
            break;
    }

    if (QFile styleFile(stylePath); styleFile.open(QFile::ReadOnly | QFile::Text)) {
        const QString style = styleFile.readAll();
        qApp->setStyleSheet(style);
        styleFile.close();
        m_currentTheme = theme;
        qDebug() << "Application theme set to:" << (theme == DarkTheme ? "Dark" : "Light");
        emit themeChanged(m_currentTheme);
    } else {
        qWarning() << "Failed to load stylesheet:" << stylePath;
    }
}

// New slot to handle palette changes
void ThemeManager::onApplicationPaletteChanged(const QPalette &oldPalette)
{
    Q_UNUSED(oldPalette); // oldPalette is not used in this simple example

    if (const AppTheme detectedTheme = getThemeFromPalette(qApp->palette()); detectedTheme != m_currentTheme) {
        qDebug() << "System palette changed. Detected new theme:" << (detectedTheme == DarkTheme ? "Dark" : "Light");
        setAppTheme(detectedTheme); // Apply the detected theme
    }
}

bool ThemeManager::event(QEvent *event)
{
    if (event->type() == QEvent::ApplicationPaletteChange) {
        qDebug() << "Received QEvent::ApplicationPaletteChange!";

        if (const AppTheme detectedTheme = getThemeFromPalette(qApp->palette()); detectedTheme != m_currentTheme) {
            qDebug() << "System palette changed. Detected new theme:" << (detectedTheme == DarkTheme ? "Dark" : "Light");
            setAppTheme(detectedTheme); // Apply the detected theme
        }
        return true; // Event handled
    }
    // Pass other events to the base class
    return QObject::event(event);
}

// Helper function to determine theme from palette
AppTheme ThemeManager::getThemeFromPalette(const QPalette &palette)
{
    // Heuristic: Check the brightness of the window background color.
    // Dark themes typically have a very dark window background.
    const QColor windowColor = palette.color(QPalette::Window);
    // Calculate relative luminance (a common way to measure brightness)
    // Formula: 0.2126*R + 0.7152*G + 0.0722*B
    // Values range from 0 (black) to 255 (white) for 8-bit color components.
    const double luminance = (0.2126 * windowColor.red() +
                        0.7152 * windowColor.green() +
                        0.0722 * windowColor.blue()) / 255.0;

    // A common threshold; adjust as needed. Values below this are considered "dark".

    if (constexpr double DARK_THEME_LUMINANCE_THRESHOLD = 0.3; luminance < DARK_THEME_LUMINANCE_THRESHOLD) {
        return DarkTheme;
    }
    return LightTheme;
}
