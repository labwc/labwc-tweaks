#include "behaviour.h"
#include "find-themes.h"
#include "macros.h"
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
    QStringList policies = { "", "Automatic", "Cascade", "Center", "Cursor" };
    ui->placementPolicy->addItems(policies);
    ui->placementPolicy->setCurrentIndex(
            policies.indexOf(getStr("/labwc_config/placement/policy")));
}

void Behaviour::onApply()
{
    setStr("/labwc_config/placement/policy", TEXT(ui->placementPolicy));
}
