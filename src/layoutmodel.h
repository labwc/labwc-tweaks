#pragma once
#include <QAbstractListModel>
#include <QString>

class Layout
{
public:
    Layout(QString code, QString desc);
    ~Layout();

private:
    QString m_code;
    QString m_desc;

public:
    QString code() const { return m_code; }
    QString desc() const { return m_desc; }
};

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
    QVector<QSharedPointer<Layout>> m_layouts;
};
