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

    connect(ui->sourceXBox, SIGNAL(valueChanged(int)), ui->plotArea,
            SLOT(emitterXChanged(int)));
    connect(ui->sourceYBox, SIGNAL(valueChanged(int)), ui->plotArea,
            SLOT(emitterYChanged(int)));
    connect(ui->sourceAngleBox, SIGNAL(valueChanged(double)), ui->plotArea,
            SLOT(emitterAngleChanged(double)));

    connect(ui->focalLengthBox, SIGNAL(valueChanged(int)), ui->plotArea,
            SLOT(lensFocalLengthChanged(int)));

    connect(ui->sourceAngleSlider, SIGNAL(valueChanged(int)),
            SLOT(angleChanged(int)));
    connect(ui->sourceAngleBox, SIGNAL(valueChanged(double)),
            SLOT(angleChanged(double)));

    connect(ui->emittersList, SIGNAL(currentRowChanged(int)), ui->plotArea,
            SLOT(setCurrentEmitter(int)));

    connect(ui->deleteEmitterButton, SIGNAL(clicked()), SLOT(deleteEmitter()));
    connect(ui->addEmitterButton, SIGNAL(clicked()), SLOT(addEmitter()));

    connect(ui->plotArea, SIGNAL(currentEmitterChanged(int)), SLOT(currentEmitterChanged(int)));

    setControlsActive(false);

    ui->plotArea->setLensFocalLength(ui->focalLengthBox->value() / 10.0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::currentEmitterChanged(int index)
{
    if (index != -1) {
        const RayEmitter& emitter = ui->plotArea->emitterAt(index);

        ui->sourceAngleBox->setValue(emitter.angle() * 180.0 / M_PI);
        ui->sourceXBox->setValue(emitter.pos().x() * 10);
        ui->sourceYBox->setValue(emitter.pos().y() * 10);

        if (ui->emittersList->currentRow() != index) {
            ui->emittersList->setCurrentRow(index);
        }
    }
}

void MainWindow::angleChanged(double angle)
{
    if (ui->sourceAngleSlider->value() != angle * 10) {
        ui->sourceAngleSlider->setValue(angle * 10);
    }
}

void MainWindow::angleChanged(int angle)
{
    if (ui->sourceAngleBox->value() != angle / 10.0) {
        ui->sourceAngleBox->setValue(angle / 10.0);
    }
}

void MainWindow::addEmitter()
{
    ui->plotArea->addEmitter();

    ui->emittersList->addItem(QString("Emitter %1").arg(ui->emittersList->count() + 1));
    ui->emittersList->setCurrentRow(ui->emittersList->count() - 1);

    setControlsActive(true);
}

void MainWindow::deleteEmitter()
{
    int row = ui->emittersList->currentRow();
    if (row != -1) {
        delete ui->emittersList->takeItem(row);

        ui->plotArea->removeEmitter(row);
    }

    if (ui->emittersList->count() == 0) {
        setControlsActive(false);
    }
}

void MainWindow::setControlsActive(bool active)
{
    ui->sourcePositionLabel->setEnabled(active);

    ui->sourceXBox->setEnabled(active);
    ui->sourceXSlider->setEnabled(active);
    ui->sourceXLabel->setEnabled(active);

    ui->sourceXBox->setEnabled(active);
    ui->sourceXSlider->setEnabled(active);
    ui->sourceXLabel->setEnabled(active);

    ui->sourceYBox->setEnabled(active);
    ui->sourceYSlider->setEnabled(active);
    ui->sourceYLabel->setEnabled(active);

    ui->sourceAngleBox->setEnabled(active);
    ui->sourceAngleSlider->setEnabled(active);
    ui->sourceAngleLabel->setEnabled(active);
}
