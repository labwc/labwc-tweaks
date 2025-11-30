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
}

void Behaviour::onApply()
{
    setStr("/labwc_config/placement/policy", DATA(ui->placementPolicy));
}
