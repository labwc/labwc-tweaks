#pragma once
#include <QAbstractListModel>
#include <QString>
#include "pair.h"

class LayoutModel : public QAbstractListModel
{
    Q_OBJECT

public:
    LayoutModel(QObject *parent = nullptr);
    ~LayoutModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    void addLayout(const QString &code, const QString &desc);
    void deleteLayout(int index);
    QString getXkbDefaultLayout();

private:
    void update(void);
    QVector<QSharedPointer<Pair>> m_layouts;
};
