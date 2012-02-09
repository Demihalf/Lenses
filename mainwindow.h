#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void emitterChanged();

private:
    Ui::MainWindow *ui;

    static const int kScale = 50;
};

#endif // MAINWINDOW_H
