#include "maindialog.h"

#include <QApplication>
#include <QLibraryInfo>
#include <QTranslator>

static void initLocale(QTranslator* qtTranslator, QTranslator* translator)
{
    QApplication* app = qApp;
#if PROJECT_TRANSLATION_TEST_ENABLED
    QLocale locale(QLocale(PROJECT_TRANSLATION_TEST_LANGUAGE));
    QLocale::setDefault(locale);
#else
    QLocale locale = QLocale::system();
#endif
    // Qt translations (buttons text and the like)
    QString translationsPath     = QLibraryInfo::path(QLibraryInfo::TranslationsPath);
    QString translationsFileName = QStringLiteral("qt_") + locale.name();

    if (qtTranslator->load(translationsFileName, translationsPath))
        app->installTranslator(qtTranslator);

    translationsFileName = QString(PROJECT_ID) + '_' + locale.name(); // E.g. "<appname>_en"

    // Try first in the same binary directory, in case we are building,
    // otherwise read from system data
    translationsPath = QCoreApplication::applicationDirPath();

    bool isLoaded = translator->load(translationsFileName, translationsPath);
    if (!isLoaded) {
        // "/usr/share/<appname>/translations
        isLoaded = translator->load(translationsFileName,
            QStringLiteral(PROJECT_DATA_DIR) + QStringLiteral("/translations"));
    }
    app->installTranslator(translator);
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(PROJECT_ID);

    QTranslator qtTranslator, translator;
    initLocale(&qtTranslator, &translator);

    MainDialog w;
    w.show();

    // Make work the window icon also when the application is not (yet) installed
    QString iconSuffix   = QString("%1%2%3").arg("/", PROJECT_APPSTREAM_ID, QStringLiteral(".svg"));
    QString icoLocalPath = QCoreApplication::applicationDirPath() + iconSuffix;
    QString icoSysPath   = QStringLiteral(PROJECT_ICON_SYSTEM_PATH) + iconSuffix;

    QIcon appIcon = QIcon(icoLocalPath);
    if (appIcon.isNull())
        appIcon = QIcon(icoSysPath);

    w.setWindowIcon(appIcon);

    return app.exec();
}
