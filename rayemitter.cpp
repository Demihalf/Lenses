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

#include "rayemitter.h"

#include <qmath.h>

RayEmitter::RayEmitter(const QPointF &pos, qreal angle)
    : m_position(pos), m_angle(angle)
{
}

QPointF RayEmitter::pos() const
{
    return m_position;
}

void RayEmitter::setPos(const QPointF &pos)
{
    if (pos != m_position) {
        m_position = pos;
    }
}

qreal RayEmitter::angle() const
{
    return m_angle;
}

void RayEmitter::setAngle(qreal angle)
{
    if (angle != m_angle) {
        m_angle = angle;
    }
}

qreal RayEmitter::slope() const
{
    return qTan(m_angle);
}

void RayEmitter::setSlope(qreal slope)
{
    m_angle = qAtan(slope);
}

QColor RayEmitter::color() const
{
    return m_color;
}

void RayEmitter::setColor(const QColor &color)
{
    m_color = color;
}

QPointF RayEmitter::planeIntersection(qreal planeX) const
{
    return QPointF(planeX, slope() * (planeX - pos().x()) + pos().y());
}
