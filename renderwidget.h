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

#ifndef RENDERWIDGET_H
#define RENDERWIDGET_H

#include <QWidget>
#include <QVector2D>

#include "rayemitter.h"

class RenderWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RenderWidget(QWidget *parent = 0);

    const RayEmitter& emitterAt(int index) const;

    void setLensFocalLength(qreal len);
    qreal lensFocalLength() const;

public slots:
    void emitterXChanged(int newValue);
    void emitterYChanged(int newValue);
    void emitterAngleChanged(double newValue);
    void lensFocalLengthChanged(int newValue);

    void addEmitter();
    void removeEmitter(int index);
    void setCurrentEmitter(int index);

signals:
    void currentEmitterChanged(int index);

protected:
    void paintEvent(QPaintEvent *event);
    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private:
    void paintAxis(QPainter &p);
    void paintLens(QPainter &p);
    void paintEmitter(QPainter &p, const QPoint &pos, bool selected);
    void paintRay(QPainter &p, const RayEmitter &emitter);

    inline QPoint cartesianToInternal(const QPointF &point);
    inline QPointF internalToCartesian(const QPoint &point);

    QList<RayEmitter> m_emitters;
    qreal m_focalLength;

    QPoint m_offset;
    qreal m_scalingFactor;

    int m_currentEmitter;

    bool m_dragging;
    bool m_draggingEmitter;
    QPoint m_lastMousePos;

    int m_lastColor;
};

#endif // RENDERWIDGET_H
