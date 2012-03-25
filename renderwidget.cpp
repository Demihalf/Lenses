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

#include <cmath>

const qreal kMoveStep = 0.1;
const qreal kZoomStep = 0.5;
const int kEmitterRadius = 5;
const qreal kScalingFactor = 10.0;

const QPointF kDefaultPos(-25, 15);

QList<Qt::GlobalColor> kColors;

RenderWidget::RenderWidget(QWidget *parent) :
    QWidget(parent),
    m_offset(0, 0),
    m_scalingFactor(kScalingFactor),
    m_currentEmitter(-1),
    m_dragging(false),
    m_draggingEmitter(false),
    m_lastColor(0)
{
   kColors <<
       Qt::red<<
       Qt::green<<
       Qt::blue<<
       Qt::cyan<<
       Qt::magenta<<
       Qt::yellow<<
       Qt::gray<<
       Qt::darkRed<<
       Qt::darkGreen<<
       Qt::darkBlue<<
       Qt::darkCyan<<
       Qt::darkMagenta<<
       Qt::darkYellow<<
       Qt::darkGray<<
       Qt::black;

    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
}

const RayEmitter& RenderWidget::emitterAt(int index) const
{
    return m_emitters.at(index);
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

inline QPoint RenderWidget::cartesianToInternal(const QPointF &point)
{
    QPoint newPoint((this->width() / 2) + point.x() * m_scalingFactor,
                    (this->height() / 2) - point.y() * m_scalingFactor);

    return newPoint;
}

inline QPointF RenderWidget::internalToCartesian(const QPoint &point)
{
    QPointF newPoint((point.x() - this->width() / 2) / m_scalingFactor,
                     (this->height() / 2 - point.y()) / m_scalingFactor);
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

void RenderWidget::paintEmitter(QPainter &p, const QPoint &pos, bool selected)
{
    p.save();

    p.setPen(QPen(QBrush(Qt::red), 1));

    if (selected) {
        p.setBrush(QBrush(Qt::green));
    } else {
        p.setBrush(QBrush(Qt::red));
    }

    p.drawEllipse(pos, kEmitterRadius, kEmitterRadius);

    p.restore();
}

void RenderWidget::paintRay(QPainter &p, const RayEmitter &emitter)
{
    p.save();

    p.setPen(QPen(QBrush(emitter.color()), 2));

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

        p.setPen(QPen(QBrush(emitter.color()), 1, Qt::DashLine));
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

    for (int i = 0; i < m_emitters.size(); i++) {
        paintEmitter(p, cartesianToInternal(m_emitters.at(i).pos()), i == m_currentEmitter);
        paintRay(p, m_emitters.at(i));
    }
}

void RenderWidget::wheelEvent(QWheelEvent *event)
{
    if (m_scalingFactor + event->delta() / 8 / 15 * kZoomStep > 0)
        m_scalingFactor += event->delta() / 8 / 15 * kZoomStep;
    update();
}

void RenderWidget::keyPressEvent(QKeyEvent *event)
{
    if (m_currentEmitter != -1) {
        RayEmitter& em = m_emitters[m_currentEmitter];
        QPointF emPos = em.pos();

        const qreal angleStep = 0.5;

        switch (event->key()) {
        case Qt::Key_Left:
            emPos.setX(emPos.x() - kMoveStep);
            break;
        case Qt::Key_Right:
            if (int(emPos.x() * 10) == -1) {
                break;
            }

            emPos.setX(emPos.x() + kMoveStep);
            break;
        case Qt::Key_Down:
            if (event->modifiers().testFlag(Qt::ControlModifier)) {
                em.setAngle(em.angle() - angleStep * M_PI / 180);
            } else {
                emPos.setY(emPos.y() - kMoveStep);
            }

            break;
        case Qt::Key_Up:
            if (event->modifiers().testFlag(Qt::ControlModifier)) {
                em.setAngle(em.angle() + angleStep * M_PI / 180);
            } else {
                emPos.setY(emPos.y() + kMoveStep);
            }

            break;
        default:
            QWidget::keyPressEvent(event);
            return;
        }

        em.setPos(emPos);

        emit currentEmitterChanged(m_currentEmitter);
    } else {
        QWidget::keyPressEvent(event);
        return;
    }

    update();
}

void RenderWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        int eventX = event->pos().x();
        int eventY = event->pos().y();

        for (int i = 0; i < m_emitters.size(); i++) {
            int emX = (cartesianToInternal(m_emitters[i].pos()) + m_offset).x();
            int emY = (cartesianToInternal(m_emitters[i].pos()) + m_offset).y();

            if (eventX < emX + 10 && eventX > emX - 10
                    && eventY < emY + 10 && eventY > emY - 10)
            {
                setCurrentEmitter(i);
                m_draggingEmitter = true;

                break;
            }
        }

        m_lastMousePos = event->pos();

        if (!m_draggingEmitter) {
            m_dragging = true;
            setCursor(QCursor(Qt::ClosedHandCursor));
        }
    } else {
        m_dragging = false;
        m_draggingEmitter = false;
        event->ignore();
    }
}

void RenderWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_dragging) {
        m_offset -= m_lastMousePos - event->pos();
        m_lastMousePos = event->pos();

        update();
    } else if (m_draggingEmitter) {
        RayEmitter& em = m_emitters[m_currentEmitter];
        QPointF emPos;
        QPoint offset = m_lastMousePos - event->pos();

        emPos = internalToCartesian(cartesianToInternal(em.pos()) - offset);

        if (static_cast<int>(emPos.x()) >= -1 && offset.x() < 0) {
            emPos.setX(-1);
        }

        m_lastMousePos = event->pos();
        em.setPos(emPos);

        emit currentEmitterChanged(m_currentEmitter);
        update();
    } else {
        event->ignore();
    }
}

void RenderWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragging = false;
        m_draggingEmitter = false;
        setCursor(QCursor(Qt::ArrowCursor));
    } else {
        event->ignore();
    }
}

void RenderWidget::emitterXChanged(int newValue)
{
    RayEmitter &emitter = m_emitters[m_currentEmitter];

    if (static_cast<int>(emitter.pos().x()) != newValue) {
        emitter.setPos(QPointF(newValue, emitter.pos().y()));

        update();
    }
}

void RenderWidget::emitterYChanged(int newValue)
{
    RayEmitter &emitter = m_emitters[m_currentEmitter];

    if (static_cast<int>(emitter.pos().y()) != newValue) {
        emitter.setPos(QPointF(emitter.pos().x(), newValue));

        update();
    }
}

void RenderWidget::emitterAngleChanged(double newValue)
{
    RayEmitter &emitter = m_emitters[m_currentEmitter];
    emitter.setAngle(newValue * M_PI / 180.0);

    update();
}

void RenderWidget::lensFocalLengthChanged(int newValue)
{
    m_focalLength = newValue;
    update();
}

void RenderWidget::addEmitter()
{
    RayEmitter em(kDefaultPos, 0);
    em.setColor(kColors[m_lastColor++ % kColors.size()]);

    m_emitters.append(em);
    update();
}

void RenderWidget::removeEmitter(int index)
{
    m_emitters.removeAt(index);
    update();
}

void RenderWidget::setCurrentEmitter(int index)
{
    m_currentEmitter = index;
    emit currentEmitterChanged(index);

    update();
}
