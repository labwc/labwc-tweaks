#include <QDebug>
#include <QVectorIterator>
#include "environment.h"
#include "layoutmodel.h"
#include "evdev-lst-layouts.h"

Layout::Layout(QString code, QString desc)
{
    m_code = code;
    m_desc = desc;
}

Layout::~Layout() { }

LayoutModel::LayoutModel(QObject *parent) : QAbstractListModel(parent)
{
    QString xkb_default_layout = environmentGet("XKB_DEFAULT_LAYOUT");
    QStringList layoutElements = xkb_default_layout.split(',');

    // We don't advise using XKB_DEFAULT_LAYOUT, but handle it just in case by adding it to the
    // respective layouts, for example like "latam(deadtilde)"
    QString xkb_default_variant = environmentGet("XKB_DEFAULT_VARIANT");
    QStringList variantElements = xkb_default_variant.split(',', Qt::KeepEmptyParts);
    int i = 0;
    foreach (QString element, variantElements) {
        if (layoutElements.size() <= i) {
            break;
        }
        // Let's not add another (variant) if one is already specified.
        if (layoutElements[i].contains("(")) {
            continue;
        }
        if (!element.isEmpty()) {
            layoutElements[i] += "(" + element + ")";
        }
        ++i;
    }

    foreach (QString element, layoutElements) {
        for (auto layout : evdev_lst_layouts) {
            if (element == QString(layout.code)) {
                addLayout(QString(layout.code), QString(layout.description));
            }
        }
    }
}

LayoutModel::~LayoutModel() { }

char *LayoutModel::getXkbDefaultLayout()
{
    QString ret;
    QVectorIterator<QSharedPointer<Layout>> iter(m_layouts);
    while (iter.hasNext()) {
        ret += iter.next().get()->code();
        if (iter.hasNext()) {
            ret += ",";
        }
    }
    return ret.toLatin1().data();
}

int LayoutModel::rowCount(const QModelIndex &parent) const
{
    return m_layouts.size();
}

QVariant LayoutModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return {};
    }

    const int row = index.row();
    switch (role) {
    case Qt::DisplayRole:
        return m_layouts.at(row)->desc() + " [" + m_layouts.at(row)->code() + "]";
    }
    return {};
}

void LayoutModel::update(void)
{
    QModelIndex topLeft = createIndex(0, 0);
    emit dataChanged(topLeft, topLeft, { Qt::DisplayRole });
}

void LayoutModel::addLayout(const QString &code, const QString &desc)
{
    QVectorIterator<QSharedPointer<Layout>> iter(m_layouts);
    while (iter.hasNext()) {
        if (iter.next().get()->code() == code) {
            qDebug() << "cannot add the same layout twice";
            return;
        }
    }

    m_layouts.append(QSharedPointer<Layout>(new Layout(code, desc)));
    update();
}

void LayoutModel::deleteLayout(int index)
{
    if (index < 0 || index >= m_layouts.size()) {
        return;
    }
    m_layouts.remove(index);
    update();
}
