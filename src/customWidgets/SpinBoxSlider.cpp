/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "SpinBoxSlider.h"

#include <QHBoxLayout>
#include <QSpinBox>
#include <QHBoxLayout>

#include "qxtspanslider.h"

SpinBoxSlider::SpinBoxSlider(QWidget *parent, ControlsFlags controlFlags)
    : QWidget(parent),
      m_spanslider(nullptr),
      m_left_spinbox(nullptr),
      m_right_spinbox(nullptr),
      m_layout(nullptr),
      m_upper_value(0),
      m_lower_value(0)
{
    m_layout = new QHBoxLayout(this);
    m_left_spinbox = new QSpinBox(this);
    m_right_spinbox = new QSpinBox(this);
    m_spanslider = new QxtSpanSlider(this);

    m_spanslider->setMinimumWidth(200);
    m_spanslider->setOrientation(Qt::Horizontal);
    m_spanslider->setTickPosition(QSlider::TicksAbove);
    m_spanslider->setHandleMovementMode(QxtSpanSlider::NoOverlapping);

    m_layout->addWidget(m_left_spinbox);
    m_layout->addWidget(m_spanslider);
    m_layout->addWidget(m_right_spinbox);

    // restrict interface
    m_left_spinbox->setVisible(false);
    m_spanslider->setVisible(false);
    m_right_spinbox->setVisible(false);

    if (controlFlags.testFlag(Controls::onlySlider)
            || controlFlags.testFlag(Controls::sliderAndSpinBoxes)) {
        connect(m_spanslider.data(), &QxtSpanSlider::lowerValueChanged,
                this, &SpinBoxSlider::setLowerValue);
        connect(m_spanslider.data(), &QxtSpanSlider::upperValueChanged,
                this, &SpinBoxSlider::setUpperValue);
        m_spanslider->setVisible(true);
    }

    if (controlFlags.testFlag(Controls::onlySpinBoxes)
            || controlFlags.testFlag(Controls::sliderAndSpinBoxes)) {
        connect(m_right_spinbox, SIGNAL(valueChanged(int)),
                this, SLOT(setUpperValue(int)));
        connect(m_left_spinbox, SIGNAL(valueChanged(int)),
                this, SLOT(setLowerValue(int)));
        m_left_spinbox->setVisible(true);
        m_right_spinbox->setVisible(true);
    }

    setLayout(m_layout);
}

SpinBoxSlider::~SpinBoxSlider()
{
    m_spanslider->deleteLater();
    m_spanslider = nullptr;

    m_left_spinbox->deleteLater();
    m_left_spinbox = nullptr;

    m_right_spinbox->deleteLater();
    m_right_spinbox = nullptr;

    m_layout->deleteLater();
    m_layout = nullptr;
}

void SpinBoxSlider::setToolTip(const QString &str)
{
    m_spanslider->setToolTip(str);
}

void SpinBoxSlider::setMaximumValue(const int value)
{
    m_spanslider->setMaximum(value);
    m_left_spinbox->setMaximum(value);
    m_right_spinbox->setMaximum(value);
}

void SpinBoxSlider::setMinimumValue(const int value)
{
    m_spanslider->setMinimum(value);
    m_left_spinbox->setMinimum(value);
    m_right_spinbox->setMinimum(value);
}

void SpinBoxSlider::setLowerValuePrivate(const int value)
{
    m_lower_value = value;
    m_spanslider->setLowerPosition(value);
    m_spanslider->setLowerValue(value);
    m_left_spinbox->setValue(value);
    emit lowerValueChanged(value);
}

void SpinBoxSlider::setUpperValuePrivate(const int value)
{
    m_upper_value = value;
    m_spanslider->setUpperPosition(value);
    m_spanslider->setUpperValue(value);
    m_right_spinbox->setValue(value);
    emit upperValueChanged(value);
}

void SpinBoxSlider::setLowerValue(const int value)
{
    if (value != m_lower_value) {
        setLowerValuePrivate(value);
    }
}

void SpinBoxSlider::setUpperValue(const int value)
{
    if (value != m_upper_value) {
        setUpperValuePrivate(value);
    }
}

void SpinBoxSlider::setTickInterval(const int stepLength)
{
    m_spanslider->setTickInterval(stepLength);
    m_left_spinbox->setSingleStep(stepLength);
    m_right_spinbox->setSingleStep(stepLength);
}
