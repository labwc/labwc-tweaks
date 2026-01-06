#include "touchscreen.h"
#include <QStringList>
#include "macros.h"
#include "pair.h"
#include "settings.h"
#include "./ui_touchscreen.h"

Touchscreen::Touchscreen(QWidget *parent) : QWidget(parent), ui(new Ui::pageTouchscreen)
{
    ui->setupUi(this);
}

Touchscreen::~Touchscreen()
{
    delete ui;
}

void Touchscreen::activate()
{
    /* Touchscreen Rotation */
    settingsAddXmlStr("/labwc_config/libinput/device/calibrationMatrix", "");
    QVector<QSharedPointer<Pair>> calibrationmatrixes;
    calibrationmatrixes.append(QSharedPointer<Pair>(new Pair("", tr(""))));
    calibrationmatrixes.append(QSharedPointer<Pair>(new Pair("1 0 0 0 1 0", tr("Normal"))));
    calibrationmatrixes.append(QSharedPointer<Pair>(new Pair("0 -1 1 1 0 0", tr("Left"))));
    calibrationmatrixes.append(QSharedPointer<Pair>(new Pair("0 1 0 -1 0 1", tr("Right"))));
    calibrationmatrixes.append(QSharedPointer<Pair>(new Pair("-1 0 1 0 -1 1", tr("Inverted"))));

    QString current_calibrationmatrix = getStr("/labwc_config/libinput/device/calibrationMatrix");
    int calibrationmatrix_index = -1;
    foreach (auto calibrationmatrix, calibrationmatrixes) {
        ui->calibrationMatrix->addItem(calibrationmatrix.get()->description(),
                                       QVariant(calibrationmatrix.get()->value()));
        ++calibrationmatrix_index;
        if (current_calibrationmatrix == calibrationmatrix.get()->value()) {
            ui->calibrationMatrix->setCurrentIndex(calibrationmatrix_index);
        }
    }
}

void Touchscreen::onApply()
{
    setStr("/labwc_config/libinput/device/calibrationMatrix", DATA(ui->calibrationMatrix));
}
