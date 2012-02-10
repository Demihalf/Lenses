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
#include <QWheelEvent>
#include <QCoreApplication>

#include <QDebug>

const int kMoveStep = 20;
const qreal kZoomStep = 1.0;
const int kEmitterRadius = 5;

RenderWidget::RenderWidget(QWidget *parent) :
    QWidget(parent),
    m_offset(0, 0),
    m_scalingFactor(40.0),
    m_dragging(false)
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
    QPoint newPoint((this->width() / 2) + point.x() * m_scalingFactor,
                    (this->height() / 2) - point.y() * m_scalingFactor);

    return newPoint;
}

void RenderWidget::paintAxis(QPainter &p)
{
    int axisY = height() / 2;
    p.drawLine(-m_offset.x(), axisY, width() - m_offset.x(), axisY);

    // Focuses
    QPoint tick1 = cartesianToInternal(QPointF(m_focalLength, 0));
    QPoint tick2 = cartesianToInternal(QPointF(-m_focalLength, 0));

    const int tickHeight = 10;

    p.drawLine(tick1.x(), tick1.y() + tickHeight,
               tick1.x(), tick1.y() - tickHeight);

    p.drawLine(tick2.x(), tick2.y() + tickHeight,
               tick2.x(), tick2.y() - tickHeight);

    // Focal planes
    p.save();

    p.setPen(Qt::DashLine);

    p.drawLine(tick1.x(), -m_offset.y(), tick1.x(), height() - m_offset.y());
    p.drawLine(tick2.x(), -m_offset.y(), tick2.x(), height() - m_offset.y());

    p.restore();

}

void RenderWidget::paintLens(QPainter &p)
{
    const int offset = 40;
    const int capWidth = 20;

    p.save();

    p.setPen(QPen(QBrush(Qt::black), 2));

    p.drawLine(width() / 2, offset,
               width() / 2, height() - offset);

    if (m_focalLength > 0) {
        p.drawLine(width() / 2, offset,
                   width() / 2 - capWidth, offset + capWidth);

        p.drawLine(width() / 2, offset,
                   width() / 2 + capWidth, offset + capWidth);

        p.drawLine(width() / 2, height() - offset,
                   width() / 2 - capWidth,
                   height() - offset - capWidth);

        p.drawLine(width() / 2, height() - offset,
                   width() / 2 + capWidth,
                   height() - offset - capWidth);
    } else {
        p.drawLine(width() / 2, offset,
                   width() / 2 - capWidth, offset - capWidth);

        p.drawLine(width() / 2, offset,
                   width() / 2 + capWidth, offset - capWidth);

        p.drawLine(width() / 2, height() - offset,
                   width() / 2 - capWidth,
                   height() - offset + capWidth);

        p.drawLine(width() / 2, height() - offset,
                   width() / 2 + capWidth,
                   height() - offset + capWidth);
    }

    p.restore();
}

void RenderWidget::paintRay(QPainter &p, const RayEmitter &emitter)
{
    p.save();

    p.setPen(QPen(QBrush(Qt::red), 1));
    p.setBrush(QBrush(Qt::red));

    p.drawEllipse(cartesianToInternal(emitter.pos()),
                  kEmitterRadius, kEmitterRadius);

    p.restore();

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
    p.translate(m_offset);
    p.setRenderHint(QPainter::Antialiasing, true);

    paintAxis(p);
    paintLens(p);

    foreach (const RayEmitter & emitter, m_emitters) {
        paintRay(p, emitter);
    }
}

void RenderWidget::wheelEvent(QWheelEvent *event)
{
    m_scalingFactor += event->delta() / 8 / 15 * kZoomStep;
    update();
}

void RenderWidget::keyReleaseEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Left:
        m_offset += QPoint(kMoveStep, 0);
        break;
    case Qt::Key_Right:
        m_offset += QPoint(-kMoveStep, 0);
        break;
    case Qt::Key_Down:
        m_offset += QPoint(0, -kMoveStep);
        break;
    case Qt::Key_Up:
        m_offset += QPoint(0, kMoveStep);
        break;
    default:
        QWidget::keyReleaseEvent(event);
        return;
    }

    update();
}

void RenderWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_lastMousePos = event->pos();
        m_dragging = true;
        setCursor(QCursor(Qt::ClosedHandCursor));
    } else {
        m_dragging = false;
        event->ignore();
    }
}

void RenderWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_dragging) {
        m_offset -= m_lastMousePos - event->pos();
        m_lastMousePos = event->pos();

        update();
    } else {
        event->ignore();
    }
}

void RenderWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragging = false;
        setCursor(QCursor(Qt::ArrowCursor));
    } else {
        event->ignore();
    }
}

