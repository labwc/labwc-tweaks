/*~
 * This is a page template. It will not show when running the application normally, but can be
 * invoked by settings LABWC_TWEAKS_SHOW_TEMPLATE=1. It is intended as a starting point for
 * developers who wish to create pages.
 *
 * By page we mean list-stack pair.
 *
 * In addition to the constructor/destructor we require two methods: activate() and onApply().
 * That's it.
 */
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
