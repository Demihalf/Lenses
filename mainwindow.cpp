/*
 Copyright (c) 2012        Valery Kharitonov <kharvd@gmail.com>

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"),
 to deal in the Software without restriction, including without limitation
 the rights to use, copy, modify, merge, publish, distribute, sublicense,
 and/or sell copies of the Software, and to permit persons to whom the Software
 is furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 DEALINGS IN THE SOFTWARE.
*/

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "rayemitter.h"

#include <qmath.h>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->m_sourceXBox, SIGNAL(valueChanged(int)),
            SLOT(emitterChanged()));
    connect(ui->m_sourceYBox, SIGNAL(valueChanged(int)),
            SLOT(emitterChanged()));
    connect(ui->m_sourceAngleBox, SIGNAL(valueChanged(double)),
            SLOT(emitterChanged()));
    connect(ui->m_focalLengthBox, SIGNAL(valueChanged(int)),
            SLOT(emitterChanged()));

    connect(ui->m_sourceAngleSlider, SIGNAL(valueChanged(int)),
            SLOT(angleChanged(int)));
    connect(ui->m_sourceAngleBox, SIGNAL(valueChanged(double)),
            SLOT(angleChanged(double)));

    emitterChanged();
}

void MainWindow::emitterChanged()
{
    qreal height = 5;
    RayEmitter top(QPointF(ui->m_sourceXBox->value() / 10.0,
                           ui->m_sourceYBox->value() / 10.0),
                    ui->m_sourceAngleBox->value() * M_PI / 180.0);
    RayEmitter bottom(top.pos() - QPointF(0, height), top.angle());

    QList<RayEmitter> emitters;

    emitters << top
             << RayEmitter(top.pos(), qAtan(top.pos().y() / top.pos().x()))
             << bottom
             << RayEmitter(bottom.pos(), qAtan(bottom.pos().y() / bottom.pos().x()));

//    emitters << RayEmitter(QPointF(ui->m_sourceXBox->value(),
//                                   ui->m_sourceYBox->value()),
//                           qAtan(ui->m_sourceYBox->value() / ui->m_sourceXBox->value()));


    ui->m_plotArea->setLensFocalLength(ui->m_focalLengthBox->value() / 10.0);

    ui->m_plotArea->setEmitters(emitters);
}

void MainWindow::angleChanged(double angle)
{
    if (ui->m_sourceAngleSlider->value() != angle * 10) {
        ui->m_sourceAngleSlider->setValue(angle * 10);
    }
}

void MainWindow::angleChanged(int angle)
{
    if (ui->m_sourceAngleBox->value() != angle / 10.0) {
        ui->m_sourceAngleBox->setValue(angle / 10.0);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
