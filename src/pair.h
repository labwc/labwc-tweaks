#pragma once
#include <QString>

class Pair
{
public:
    Pair(QString value, QString description)
    {
        m_value = value;
        m_description = description;
    };
    ~Pair() { };

private:
    QString m_value;
    QString m_description;

public:
    QString value() const { return m_value; }
    QString description() const { return m_description; }
};
