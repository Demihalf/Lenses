#ifndef RENDERWIDGET_H
#define RENDERWIDGET_H

#include <QWidget>
#include <QVector2D>

class RenderWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RenderWidget(QWidget *parent = 0);
    void setEmittingPoints(const QList<QVector2D>& points);

protected:
    void paintEvent(QPaintEvent *event);

private:
    QList<QVector2D> m_emittingPoints;
};

#endif // RENDERWIDGET_H
