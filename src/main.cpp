#include "maindialog.h"

#include <QApplication>
#include <QLibraryInfo>
#include <QMessageBox>
#include <QTranslator>
#include <QFileInfo>

#include "log.h"
#include "settings.h"

extern "C" {
#include "xml.h"
}

static void initLocale(QTranslator *qtTranslator, QTranslator *translator)
{
    QApplication *app = qApp;
#if PROJECT_TRANSLATION_TEST_ENABLED
    QLocale locale(QLocale(PROJECT_TRANSLATION_TEST_LANGUAGE));
    QLocale::setDefault(locale);
#else
    QLocale locale = QLocale::system();
#endif
    // Qt translations (buttons text and the like)
    QString translationsPath = QLibraryInfo::path(QLibraryInfo::TranslationsPath);
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
                                    QStringLiteral(PROJECT_DATA_DIR)
                                            + QStringLiteral("/translations"));
    }
    app->installTranslator(translator);
}

void initConfig(std::string &configFile)
{
    // mkdir -p
    if (!std::filesystem::exists(configFile)) {
        size_t filenamePosition = configFile.find_last_of("/");
        std::string dirname = configFile.substr(0, filenamePosition);
        info("Creating directory '{}'", dirname);
        std::filesystem::create_directories(dirname);
    }

    bool success = xml_init(configFile.data());

    if (!success) {
        QMessageBox msgBox;
        msgBox.setText(QObject::tr("Error loading ") + QString(configFile.data()));
        msgBox.setInformativeText(
                QObject::tr("Run labwc-tweaks from a terminal to view error messages"));
        msgBox.exec();
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(PROJECT_ID);

    QTranslator qtTranslator, translator;
    initLocale(&qtTranslator, &translator);

    std::string config_home = std::getenv("HOME") + std::string("/.config/labwc");
    std::string config_dir = std::getenv("LABWC_CONFIG_DIR") ?: config_home;
    std::string config_file = config_dir + "/rc.xml";
    initConfig(config_file);

    // The 'settings' vector contains the master state of all settings that can
    // be changed by labwc-tweaks.
    std::vector<std::shared_ptr<Setting>> settings;
    initSettings(settings);

    MainDialog window(settings);
    window.show();

    // Make work the window icon also when the application is not (yet) installed
    QString iconSuffix = QString("%1%2%3").arg("/", PROJECT_APPSTREAM_ID, QStringLiteral(".svg"));
    QString icoLocalPath = QCoreApplication::applicationDirPath() + iconSuffix;
    QString icoSysPath = QStringLiteral(PROJECT_ICON_SYSTEM_PATH) + iconSuffix;

    // If icoLocalPath exists, set to icolocalPath; else set to icoSysPath
    QIcon appIcon = (QFileInfo(icoLocalPath).exists()) ? QIcon(icoLocalPath) : QIcon(icoSysPath);

    window.setWindowIcon(appIcon);

    return app.exec();
}
