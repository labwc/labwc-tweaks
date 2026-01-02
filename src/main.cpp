/*~
 * Welcome, dear reader. This is the main file of labwc-tweaks, and as such, a good starting point
 * for reading the code.  Comments beginning with a tilde (~) are part of a thread running through
 * the source with the aim of shortening the route to familiarity. They are meant to be read in a
 * certain order.
 */
#include <QApplication>
#include <QFileInfo>
#include <QLibraryInfo>
#include <QMessageBox>
#include <QTranslator>
#include "environment.h"
#include "log.h"
#include "maindialog.h"
#include "settings.h"
#include "xml.h"

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

void mkdir_p(std::string path)
{
    if (!std::filesystem::exists(path)) {
        info("Creating directory '{}'", path);
        std::filesystem::create_directories(path);
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

    mkdir_p(config_dir);

    std::string environment_file = config_dir + "/environment";
    environmentInit(environment_file);

    std::string config_file = config_dir + "/rc.xml";
    initConfig(config_file);

    /*~
     * This settings vector contains the master state of all key=value type settings that can be
     * changed by labwc-tweaks.
     *
     * settings.h contains an API for working with these.
     */
    std::vector<std::shared_ptr<Setting>> settings;
    settingsInit(&settings);

    MainDialog window;
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
