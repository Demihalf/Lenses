#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "rayemitter.h"

#include <qmath.h>

MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->m_sourceXBox, SIGNAL(valueChanged(double)),
            SLOT(emitterChanged()));
    connect(ui->m_sourceYBox, SIGNAL(valueChanged(double)),
            SLOT(emitterChanged()));
    connect(ui->m_sourceAngleBox, SIGNAL(valueChanged(double)),
            SLOT(emitterChanged()));
    connect(ui->m_focalLengthBox, SIGNAL(valueChanged(double)),
            SLOT(emitterChanged()));

    QList<RayEmitter> emitters;
    emitters << RayEmitter(QPointF(ui->m_sourceXBox->value(),
                                   ui->m_sourceYBox->value()),
                           ui->m_sourceAngleBox->value() * M_PI / 180.0);
    emitters << RayEmitter(QPointF(ui->m_sourceXBox->value(),
                                   ui->m_sourceYBox->value()),
                           qAtan(ui->m_sourceYBox->value() / ui->m_sourceXBox->value()));

//    emitters << RayEmitter(QPointF(ui->m_sourceXBox->value(),
//                                   ui->m_sourceYBox->value() - 1),
//                           ui->m_sourceAngleBox->value() * M_PI / 180.0);

//    emitters << RayEmitter(QPointF(ui->m_sourceXBox->value(),
//                                   ui->m_sourceYBox->value() - 2),
//                           ui->m_sourceAngleBox->value() * M_PI / 180.0);

    ui->m_plotArea->setLensFocalLength(ui->m_focalLengthBox->value());

    ui->m_plotArea->setEmitters(emitters);
}

void MainWindow::emitterChanged()
{
    QList<RayEmitter> emitters;

    emitters << RayEmitter(QPointF(ui->m_sourceXBox->value(),
                                   ui->m_sourceYBox->value()),
                           ui->m_sourceAngleBox->value() * M_PI / 180.0);

    emitters << RayEmitter(QPointF(ui->m_sourceXBox->value(),
                                   ui->m_sourceYBox->value()),
                           qAtan(ui->m_sourceYBox->value() / ui->m_sourceXBox->value()));

//    emitters << RayEmitter(QPointF(ui->m_sourceXBox->value(),
//                                   ui->m_sourceYBox->value() - 1),
//                           ui->m_sourceAngleBox->value() * M_PI / 180.0);

//    emitters << RayEmitter(QPointF(ui->m_sourceXBox->value(),
//                                   ui->m_sourceYBox->value() - 2),
//                           ui->m_sourceAngleBox->value() * M_PI / 180.0);

    ui->m_plotArea->setLensFocalLength(ui->m_focalLengthBox->value());

    ui->m_plotArea->setEmitters(emitters);
}

MainWindow::~MainWindow()
{
    delete ui;
}
