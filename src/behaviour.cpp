#include "behaviour.h"
#include <QVectorIterator>
#include "find-themes.h"
#include "macros.h"
#include "pair.h"
#include "settings.h"
#include "./ui_behaviour.h"

Behaviour::Behaviour(QWidget *parent) : QWidget(parent), ui(new Ui::pageBehaviour)
{
    ui->setupUi(this);
}

Behaviour::~Behaviour()
{
    delete ui;
}

void Behaviour::activate()
{
    /* Placement Policy */
    settingsAddXmlStr("/labwc_config/placement/policy", "cascade");
    ui->placementPolicy->setToolTip(tr("Placement policy for new windows"));
    QVector<QSharedPointer<Pair>> policies;
    policies.append(QSharedPointer<Pair>(new Pair("automatic", tr("Automatic"))));
    policies.append(QSharedPointer<Pair>(new Pair("cascade", tr("Cascade"))));
    policies.append(QSharedPointer<Pair>(new Pair("center", tr("Center"))));
    policies.append(QSharedPointer<Pair>(new Pair("cursor", tr("Cursor"))));

    QString current = getStr("/labwc_config/placement/policy");
    int index = -1;
    foreach (auto policy, policies) {
        ui->placementPolicy->addItem(policy.get()->description(), QVariant(policy.get()->value()));
        ++index;
        if (current == policy.get()->value()) {
            ui->placementPolicy->setCurrentIndex(index);
        }
    }

    /* Focus Follow Mouse */
    settingsAddXmlBoo("/labwc_config/focus/followMouse", false);
    ui->followMouse->setChecked(getBool("/labwc_config/focus/followMouse"));
    ui->followMouse->setToolTip(tr("Focus is given to window under mouse cursor"));

    /* Focus Requires Movement */
    settingsAddXmlBoo("/labwc_config/focus/followMouseRequiresMovement", false);
    ui->followMouseRequiresMovement->setChecked(
            getBool("/labwc_config/focus/followMouseRequiresMovement"));
    ui->followMouseRequiresMovement->setToolTip(
            tr("Requires cursor movement if followMouse is enabled"));
    ui->followMouseRequiresMovement->setEnabled(ui->followMouse->isChecked());
    connect(ui->followMouse, &QCheckBox::toggled, ui->followMouseRequiresMovement,
            &QWidget::setEnabled);

    /* Raise on Focus */
    settingsAddXmlBoo("/labwc_config/focus/raiseOnFocus", false);
    ui->raiseOnFocus->setChecked(getBool("/labwc_config/focus/raiseOnFocus"));
    ui->raiseOnFocus->setToolTip(tr("Raise window to front when focused"));

    /* Gap (Core) */
    settingsAddXmlInt("/labwc_config/core/gap", 0);
    ui->gap->setValue(getInt("/labwc_config/core/gap"));
    ui->gap->setToolTip(
            tr("Distance between windows and output edges when using movement actions"));

    /* Snapping Corner Range */
    settingsAddXmlInt("/labwc_config/snapping/cornerRange", 50);
    ui->snapCornerRange->setValue(getInt("/labwc_config/snapping/cornerRange"));
    ui->snapCornerRange->setToolTip(tr(""));

    /* Maximize On Top */
    settingsAddXmlBoo("/labwc_config/snapping/topMaximize", true);
    ui->topMaximize->setChecked(getBool("/labwc_config/snapping/topMaximize"));
    ui->topMaximize->setToolTip(tr("Maximize instead of snapping on top edge"));

    /* Notify Clients */
    settingsAddXmlStr("/labwc_config/snapping/notifyClient", "always");
    ui->notifyClients->setToolTip(tr("Snapping windows can trigger corresponding\ntiling events "
                                     "for native Wayland applications"));
    QVector<QSharedPointer<Pair>> notifyclients;
    notifyclients.append(QSharedPointer<Pair>(new Pair("always", tr("Always"))));
    notifyclients.append(QSharedPointer<Pair>(new Pair("region", tr("Only on regions"))));
    notifyclients.append(QSharedPointer<Pair>(new Pair("edge", tr("Only on edges"))));
    notifyclients.append(QSharedPointer<Pair>(new Pair("never", tr("Never"))));

    QString current_notifyclientvalue = getStr("/labwc_config/snapping/notifyClient");
    int notifyclientvalue_index = -1;
    foreach (auto notifyclientvalue, notifyclients) {
        ui->notifyClients->addItem(notifyclientvalue.get()->description(),
                                   QVariant(notifyclientvalue.get()->value()));
        ++notifyclientvalue_index;
        if (current_notifyclientvalue == notifyclientvalue.get()->value()) {
            ui->notifyClients->setCurrentIndex(notifyclientvalue_index);
        }
    }

    // clang-format off

    /* Resistance: Screen Edge Strength */
    settingsAddXmlInt("/labwc_config/resistance/screenEdgeStrength", 20);
    ui->screenEdgeStrength->setValue(getInt("/labwc_config/resistance/screenEdgeStrength"));
    ui->screenEdgeStrength->setToolTip(tr("Resist interactive moves and resizes of a window\n across screen edges"));

    /* Window Edge Strength */
    settingsAddXmlInt("/labwc_config/resistance/windowEdgeStrength", 20);
    ui->windowEdgeStrength->setValue(getInt("/labwc_config/resistance/windowEdgeStrength"));
    ui->windowEdgeStrength->setToolTip(tr("Resist interactive moves and resizes of a window\n across the edges of any other window"));

    /* resistance UnSnap Treshold */
    settingsAddXmlInt("/labwc_config/resistance/unSnapTreshold", 20);
    ui->unSnapTreshold->setValue(getInt("/labwc_config/resistance/unSnapTreshold"));
    ui->unSnapTreshold->setToolTip(tr("Movement of cursor required for a tiled or maximized window to be moved"));

    /* resistance UnMaximizeTreshold */
    settingsAddXmlInt("/labwc_config/resistance/unMaximizeTreshold", 150);
    ui->unMaximizeTreshold->setValue(getInt("/labwc_config/resistance/unMaximizeTreshold"));
    ui->unMaximizeTreshold->setToolTip(tr("One-dimensional movement of cursor required for\na vertically or horizontally maximized window to be moved"));

    /* Resize: drawContents */
    settingsAddXmlBoo("/labwc_config/resize/drawContents", true);
    ui->drawContents->setChecked(getBool("/labwc_config/resize/drawContents"));
    ui->drawContents->setToolTip(tr("Application redraws its contents while resizing.\nIf "
                                    "disabled, an outlined rectangle is shown"));

    /* Keep Border (under 'theme') */
    settingsAddXmlBoo("/labwc_config/theme/keepBorder", true);
    ui->keepBorder->setChecked(getBool("/labwc_config/theme/keepBorder"));
    ui->keepBorder->setToolTip(
            tr("Even when disabling server side decorations via ToggleDecorations,\nkeep a small "
               "border (and resize area) around the window"));

    /* Resize: Corner Range */
    settingsAddXmlInt("/labwc_config/resize/cornerRange", 8);
    ui->resizeCornerRange->setValue(getInt("/labwc_config/resize/cornerRange"));
    ui->resizeCornerRange->setToolTip(
            tr("Size of corner regions to which all 'Corner' mousebinds contexts apply\n as well "
               "size of border region for which mouse resizing will apply in any direction."));

    /* Resize Minimum Area */
    settingsAddXmlInt("/labwc_config/resize/resizeMinimumArea", 8);
    ui->resizeMinimumArea->setValue(getInt("/labwc_config/resize/resizeMinimumArea"));
    ui->resizeMinimumArea->setToolTip(tr("Treat borders of server-side decorations as \nif they "
                                         "were at least the indicated thickness"));

    // clang-format on

    /* Show Popup */
    settingsAddXmlStr("/labwc_config/resize/popupShow", "never");
    ui->popupShow->setToolTip(
            tr("Show a small indicator on top of the window when resizing or moving"));
    QVector<QSharedPointer<Pair>> resizeShowPopup;
    resizeShowPopup.append(QSharedPointer<Pair>(new Pair("never", tr("Never"))));
    resizeShowPopup.append(QSharedPointer<Pair>(new Pair("always", tr("Always"))));
    resizeShowPopup.append(QSharedPointer<Pair>(new Pair("nonpixel", tr("Nonpixel"))));

    QString current_popupValue = getStr("/labwc_config/resize/popupShow");
    int popupValue_index = -1;
    foreach (auto popupValue, resizeShowPopup) {
        ui->popupShow->addItem(popupValue.get()->description(),
                               QVariant(popupValue.get()->value()));
        ++popupValue_index;
        if (current_popupValue == popupValue.get()->value()) {
            ui->popupShow->setCurrentIndex(popupValue_index);
        }
    }

    /* Magnifier */
    settingsAddXmlInt("/labwc_config/magnifier/width", 400);
    ui->magnifierWidth->setValue(getInt("/labwc_config/magnifier/width"));
    ui->magnifierWidth->setToolTip(tr("For full screen magnifier set to -1"));

    settingsAddXmlInt("/labwc_config/magnifier/height", 400);
    ui->magnifierHeight->setValue(getInt("/labwc_config/magnifier/height"));
    ui->magnifierHeight->setToolTip(tr("For full screen magnifier set to -1"));

    settingsAddXmlFlt("/labwc_config/magnifier/initScale", 2.0f);
    ui->initScale->setValue(getFloat("/labwc_config/magnifier/initScale"));
    ui->initScale->setToolTip(tr("Initial number of times by which magnified image is scaled"));

    settingsAddXmlFlt("/labwc_config/magnifier/increment", 0.2f);
    ui->increment->setValue(getFloat("/labwc_config/magnifier/increment"));
    ui->increment->setToolTip(tr("Steps for changes on each call to 'ZoomIn' or 'ZoomOut'"));

    settingsAddXmlBoo("/labwc_config/magnifier/useFilter", true);
    ui->useFilter->setChecked(getBool("/labwc_config/magnifier/useFilter"));
    ui->useFilter->setToolTip(tr("Apply a bilinear filter to the magnified image"));
}

void Behaviour::onApply()
{
    setStr("/labwc_config/placement/policy", DATA(ui->placementPolicy));
    setBool("/labwc_config/focus/followMouse", ui->followMouse->isChecked());
    setBool("/labwc_config/focus/followMouseRequiresMovement",
            ui->followMouseRequiresMovement->isChecked());
    setBool("/labwc_config/focus/raiseOnFocus", ui->raiseOnFocus->isChecked());
    setInt("/labwc_config/core/gap", ui->gap->value());
    setInt("/labwc_config/snapping/cornerRange", ui->snapCornerRange->value());
    setBool("/labwc_config/snapping/topMaximize", ui->topMaximize->isChecked());
    setStr("/labwc_config/snapping/notifyClient", DATA(ui->notifyClients));
    setInt("/labwc_config/resistance/screenEdgeStrength", ui->screenEdgeStrength->value());
    setInt("/labwc_config/resistance/windowEdgeStrength", ui->windowEdgeStrength->value());
    setInt("/labwc_config/resistance/unSnapTreshold", ui->unSnapTreshold->value());
    setInt("/labwc_config/resistance/unMaximizeTreshold", ui->unMaximizeTreshold->value());
    setBool("/labwc_config/resize/drawContents", ui->drawContents->isChecked());
    setBool("/labwc_config/theme/keepBorder", ui->keepBorder->isChecked());
    setInt("/labwc_config/resize/cornerRange", ui->resizeCornerRange->value());
    setInt("/labwc_config/resize/resizeMinimumArea", ui->resizeMinimumArea->value());
    setStr("/labwc_config/resize/popupShow", DATA(ui->popupShow));
    setInt("/labwc_config/magnifier/width", ui->magnifierWidth->value());
    setInt("/labwc_config/magnifier/height", ui->magnifierHeight->value());
    setFloat("/labwc_config/magnifier/initScale", ui->initScale->value());
    setFloat("/labwc_config/magnifier/increment", ui->increment->value());
    setBool("/labwc_config/magnifier/useFilter", ui->useFilter->isChecked());
}
