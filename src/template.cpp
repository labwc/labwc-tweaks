#include "template.h"
#include <QStringList>
#include "./ui_template.h"

Template::Template(QWidget *parent) : QWidget(parent), ui(new Ui::pageTemplate)
{
    ui->setupUi(this);
}

Template::~Template()
{
    delete ui;
}

void Template::activate()
{
    QStringList items = { "Foo", "Bar", "Baz" };
    ui->comboBox->addItems(items);

    for (int i = 0; i < 100; ++i) {
        QString text = QString("Label ") + QString::number(i);
        ui->groupBox2_gridLayout->addWidget(new QLabel(text));
    }
}

void Template::onApply()
{
    // No-op
}
