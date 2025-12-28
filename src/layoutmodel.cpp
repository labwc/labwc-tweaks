#include <QVectorIterator>
#include "environment.h"
#include "layoutmodel.h"
#include "log.h"
#include "evdev-lst-layouts.h"

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
            if (element == layout.code) {
                addLayout(layout.code, layout.description);
            }
        }
    }
}

LayoutModel::~LayoutModel() { }

QString LayoutModel::getXkbDefaultLayout()
{
    QString ret;
    QVectorIterator<QSharedPointer<Pair>> iter(m_layouts);
    while (iter.hasNext()) {
        ret += iter.next().get()->value();
        if (iter.hasNext()) {
            ret += ",";
        }
    }
    return ret;
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
        return m_layouts.at(row)->description() + " [" + m_layouts.at(row)->value() + "]";
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
    QVectorIterator<QSharedPointer<Pair>> iter(m_layouts);
    while (iter.hasNext()) {
        if (iter.next().get()->value() == code) {
            warn("cannot add the same layout twice");
            return;
        }
    }

    m_layouts.append(QSharedPointer<Pair>(new Pair(code, desc)));
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
