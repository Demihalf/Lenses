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

#include "renderwidget.h"

#include <QPainter>
#include <QBrush>

RenderWidget::RenderWidget(QWidget *parent) :
    QWidget(parent)
{
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
}

void RenderWidget::setEmitters(const QList<RayEmitter>& points)
{
    m_emitters = points;
    update();
}

QList<RayEmitter> RenderWidget::emitters() const
{
    return m_emitters;
}

void RenderWidget::setLensFocalLength(qreal len)
{
    if (m_focalLength != len) {
        m_focalLength = len;
        update();
    }
}

qreal RenderWidget::lensFocalLength() const
{
    return m_focalLength;
}

QPoint RenderWidget::cartesianToInternal(const QPointF &point)
{
    QPoint newPoint((this->width() / 2) + point.x() * kScalingFactor,
                    (this->height() / 2) - point.y() * kScalingFactor);

    return newPoint;
}

void RenderWidget::paintAxis(QPainter &p)
{
    int axisY = height() / 2;
    p.drawLine(0, axisY, width(), axisY);

    // Focuses
    QPoint tick1 = cartesianToInternal(QPointF(m_focalLength, 0));
    QPoint tick2 = cartesianToInternal(QPointF(-m_focalLength, 0));

    const int tickHeight = 20;

    p.drawLine(tick1.x(), tick1.y() + tickHeight / 2,
               tick1.x(), tick1.y() - tickHeight / 2);
    p.drawLine(tick2.x(), tick2.y() + tickHeight / 2,
               tick2.x(), tick2.y() - tickHeight / 2);

    // Focal planes
    p.save();

    p.setPen(Qt::DashLine);

    p.drawLine(tick1.x(), 0, tick1.x(), height());
    p.drawLine(tick2.x(), 0, tick2.x(), height());

    p.restore();

}

void RenderWidget::paintLens(QPainter &p)
{
    const int offset = 40;
    const int capWidth = 20;

    p.save();

    p.setPen(QPen(QBrush(Qt::black), 2));

    p.drawLine(width() / 2, offset, width() / 2, height() - offset);

    if (m_focalLength > 0) {
        p.drawLine(width() / 2, offset,
                   width() / 2 - capWidth, offset + capWidth);
        p.drawLine(width() / 2, offset,
                   width() / 2 + capWidth, offset + capWidth);

        p.drawLine(width() / 2, height() - offset,
                   width() / 2 - capWidth, height() - offset - capWidth);
        p.drawLine(width() / 2, height() - offset,
                   width() / 2 + capWidth, height() - offset - capWidth);
    } else {
        p.drawLine(width() / 2, offset,
                   width() / 2 - capWidth, offset - capWidth);
        p.drawLine(width() / 2, offset,
                   width() / 2 + capWidth, offset - capWidth);

        p.drawLine(width() / 2, height() - offset,
                   width() / 2 - capWidth, height() - offset + capWidth);
        p.drawLine(width() / 2, height() - offset,
                   width() / 2 + capWidth, height() - offset + capWidth);
    }

    p.restore();
}

void RenderWidget::paintRay(QPainter &p, const RayEmitter &emitter)
{
    p.save();

    p.setPen(QPen(QBrush(Qt::blue), 2));

    RayEmitter parallel(QPointF(0.0, 0.0), emitter.angle());

    QPointF lensIntersect = emitter.planeIntersection(0.0);
    QPointF focalPlaneIntersect
            = parallel.planeIntersection(m_focalLength);

    qreal newSlope = double(focalPlaneIntersect.y() - lensIntersect.y()) /
                     (focalPlaneIntersect.x() - lensIntersect.x());

    qreal infX = 20 * qAbs(m_focalLength);
    QPointF inf = QPointF(infX, newSlope * infX + lensIntersect.y());

    QPolygon ray;

    ray << cartesianToInternal(emitter.pos())
        << cartesianToInternal(lensIntersect);

    if (m_focalLength > 0) {
        ray << cartesianToInternal(focalPlaneIntersect);
    } else {
        p.save();

        p.setPen(QPen(QBrush(Qt::blue), 1, Qt::DashLine));
        p.drawLine(cartesianToInternal(lensIntersect),
                   cartesianToInternal(focalPlaneIntersect));

        p.restore();
    }

    ray << cartesianToInternal(inf);

    p.drawPolyline(ray);

    p.restore();
}

void RenderWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    paintAxis(p);
    paintLens(p);

    foreach (const RayEmitter & emitter, m_emitters) {
        paintRay(p, emitter);
    }
}
