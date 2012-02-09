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

#ifndef RAYEMITTER_H
#define RAYEMITTER_H

#include <QPointF>

// Point that emits light in one direction
class RayEmitter
{
public:
    RayEmitter(const QPointF &pos, qreal angle);

    QPointF pos() const;
    void setPos(const QPointF &pos);

    // Angle in radians
    qreal angle() const;
    void setAngle(qreal angle);

    // tan(angle)
    qreal slope() const;
    void setSlope(qreal slope);

    // Coordinates of intersection with a line which is parallel to y-axis
    QPointF planeIntersection(qreal planeX) const;

private:
    QPointF m_position;
    qreal m_angle;
};

#endif // RAYEMITTER_H
