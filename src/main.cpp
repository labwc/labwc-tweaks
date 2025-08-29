#include "maindialog.h"

#include <QApplication>
#include <QLibraryInfo>
#include <QMessageBox>
#include <QTranslator>
#include <QFileInfo>

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

void initConfig(std::string &config_file)
{
    bool success = xml_init(config_file.data());

    if (!success) {
        QMessageBox msgBox;
        msgBox.setText(QObject::tr("Error loading ") + QString(config_file.data()));
        msgBox.setInformativeText(
                QObject::tr("Run labwc-tweaks from a terminal to view error messages"));
        msgBox.exec();
        exit(EXIT_FAILURE);
    }

    /* Ensure all relevant nodes exist before we start getting/setting */
    xpath_add_node("/labwc_config/theme/cornerRadius");
    xpath_add_node("/labwc_config/theme/name");
    xpath_add_node("/labwc_config/theme/dropShadows");
    xpath_add_node("/labwc_config/theme/icon");
    xpath_add_node("/labwc_config/placement/policy");
    xpath_add_node("/labwc_config/libinput/device/naturalScroll");

    xml_save();
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(PROJECT_ID);

    QTranslator qtTranslator, translator;
    initLocale(&qtTranslator, &translator);

    std::string config_dir =
            std::getenv("LABWC_CONFIG_DIR") ?: std::getenv("HOME") + std::string("/.config/labwc");
    std::string config_file = config_dir + "/rc.xml";
    initConfig(config_file);

    // The 'settings' vector contains the master state of all settings that can
    // be changed by labwc-tweaks.
    std::vector<std::shared_ptr<Setting>> settings;
    initSettings(settings);

    MainDialog w(settings);
    w.show();

    // Make work the window icon also when the application is not (yet) installed
    QString iconSuffix = QString("%1%2%3").arg("/", PROJECT_APPSTREAM_ID, QStringLiteral(".svg"));
    QString icoLocalPath = QCoreApplication::applicationDirPath() + iconSuffix;
    QString icoSysPath = QStringLiteral(PROJECT_ICON_SYSTEM_PATH) + iconSuffix;

    // If icoLocalPath exists, set to icolocalPath; else set to icoSysPath
    QIcon appIcon = (QFileInfo(icoLocalPath).exists()) ? QIcon(icoLocalPath) : QIcon(icoSysPath);

    w.setWindowIcon(appIcon);

    return app.exec();
}
