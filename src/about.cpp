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
#include "about.h"
#include <QStringList>
#include "./ui_about.h"

About::About(QWidget *parent) : QWidget(parent), ui(new Ui::pageAbout)
{
    ui->setupUi(this);
}

About::~About()
{
    delete ui;
}


void About::onApply()
{
    // No-op
}
