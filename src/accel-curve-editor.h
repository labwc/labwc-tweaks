// SPDX-License-Identifier: GPL-2.0-only
#pragma once
#include <QWidget>

class QGroupBox;
class QLabel;
class QLineEdit;

class AccelCurveEditor : public QWidget
{
    Q_OBJECT

public:
    AccelCurveEditor(const QString &name, const QString &title, QWidget *parent = nullptr);
    void activate();
    void onApply();
    bool isValid() const;

signals:
    void validityChanged();

private:
    void updateValidation();

    QString m_path;
    QGroupBox *m_defined;
    QLineEdit *m_step;
    QLineEdit *m_points;
    QLabel *m_error;
    bool m_wasDefined = false;
    QString m_originalStep;
    QString m_originalPoints;
};
