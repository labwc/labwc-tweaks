// SPDX-License-Identifier: GPL-2.0-only
#include "accel-curve-editor.h"
#include "accel-curve.h"
#include "settings.h"
#include "xml.h"
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>

AccelCurveEditor::AccelCurveEditor(const QString &name, const QString &title, QWidget *parent)
    : QWidget(parent), m_path("/labwc_config/libinput/device/" + name)
{
    setObjectName(name);
    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    m_defined = new QGroupBox(title, this);
    m_defined->setObjectName("defined");
    m_defined->setCheckable(true);
    m_defined->setChecked(false);
    layout->addWidget(m_defined);

    auto form = new QFormLayout(m_defined);
    // Text retains the full double range and precision, including scientific notation.
    // A spin box would silently round or clamp existing configurations.
    m_step = new QLineEdit(m_defined);
    m_step->setObjectName("step");
    m_step->setToolTip(tr("Positive spacing between input-speed samples."));
    form->addRow(tr("Step"), m_step);
    m_points = new QLineEdit(m_defined);
    m_points->setObjectName("points");
    m_points->setToolTip(tr("2 to 32 non-negative output speeds, separated by spaces. "
                           "Labwc interpolates between samples and extrapolates beyond the last two."));
    form->addRow(tr("Points"), m_points);
    m_error = new QLabel(m_defined);
    m_error->setWordWrap(true);
    m_error->setObjectName("error");
    form->addRow(m_error);

    connect(m_defined, &QGroupBox::toggled, this, &AccelCurveEditor::updateValidation);
    connect(m_step, &QLineEdit::textChanged, this, &AccelCurveEditor::updateValidation);
    connect(m_points, &QLineEdit::textChanged, this, &AccelCurveEditor::updateValidation);
}

void AccelCurveEditor::activate()
{
    settingsAddXmlStr(m_path + "/step", "");
    settingsAddXmlStr(m_path + "/points", "");
    m_wasDefined = xml_node_exists(m_path.toStdString().c_str());
    m_originalStep = m_wasDefined ? getStr(m_path + "/step") : "1";
    m_originalPoints = m_wasDefined ? getStr(m_path + "/points") : "0 1";
    m_step->setText(m_originalStep);
    m_points->setText(m_originalPoints);
    m_defined->setChecked(m_wasDefined);
    updateValidation();
}

bool AccelCurveEditor::isValid() const
{
    return !m_defined->isChecked()
            || validateAccelCurve(m_step->text().toStdString(), m_points->text().toStdString())
                    == AccelCurveError::None;
}

void AccelCurveEditor::updateValidation()
{
    QString error;
    if (m_defined->isChecked()) {
        switch (validateAccelCurve(m_step->text().toStdString(), m_points->text().toStdString())) {
        case AccelCurveError::Step:
            error = tr("Step must be a positive, finite number.");
            break;
        case AccelCurveError::Points:
            error = tr("Enter 2 to 32 finite, non-negative numbers separated by spaces.");
            break;
        case AccelCurveError::None:
            break;
        }
    }
    m_error->setText(error);
    m_error->setVisible(!error.isEmpty());
    emit validityChanged();
}

void AccelCurveEditor::onApply()
{
    if (!isValid()) {
        return;
    }
    const bool defined = m_defined->isChecked();
    if (defined == m_wasDefined && (!defined
            || (m_step->text() == m_originalStep && m_points->text() == m_originalPoints))) {
        return;
    }

    // Leave untouched curves alone. Write edited curves as child elements, without
    // retaining stale attributes or duplicate fields from the previous form.
    settingsRemoveXml(m_path);
    settingsAddXmlStr(m_path + "/step", "");
    settingsAddXmlStr(m_path + "/points", "");
    if (defined) {
        setStr(m_path + "/step", m_step->text());
        setStr(m_path + "/points", m_points->text());
    }
    m_wasDefined = defined;
    m_originalStep = m_step->text();
    m_originalPoints = m_points->text();
}
