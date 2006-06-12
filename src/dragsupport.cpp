/*
 * dragsupport.cpp - part of abakus
 * Copyright (C) 2004, 2005, 2006 Michael Pyne <michael.pyne@kdemail.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <QPixmap>
#include <QImage>
#include <QPainter>
#include <QPointF>
#include <QPainterPath>
#include <QColor>
#include <QFont>
#include <QBrush>
#include <QFontMetrics>
#include <QRectF>

#include "dragsupport.h"

/**
 * Draws a rectangle with rounded corners to fill rectangle r.  Unlike
 * QPainter::drawRoundRect, the rounded corners have the same radius in both
 * the x and y directions.
 *
 * The current pen is used for stroking the line, and the rectangle is
 * filled using the current brush.
 */
static void drawCircularRoundRect(QPainter &p, const QRect &r, int radius)
{
    QRectF rect, rf(r);
    QPointF start(rf.topLeft());

    // Move down by radius.
    start.ry() += radius + 1;

    QPainterPath path(start);

    // This stuff doesn't seem to line up exactly right, but whatever.  It's
    // close enough.

    rect = QRectF(r.left(), r.top(), 2 * radius, 2 * radius);
    path.arcTo(rect, 180.0, -90.0);
    path.lineTo(r.right() - radius, r.top());

    rect.setHeight(rect.height() - 1);
    rect.translate(rf.width() - 2 * radius, 0.0);
    path.arcTo(rect, 90.0, -90.0);
    path.lineTo(r.right() + 1, r.bottom() - radius + 1);

    rect.setWidth(rect.width() - 1);
    rect.translate(1.0, rf.height() - 2 * radius + 1);
    path.arcTo(rect, 0.0, -90.0);
    path.lineTo(r.left() + radius - 1, r.bottom() + 1);

    rect.setHeight(rect.height() + 1);
    rect.setWidth(rect.width() + 1);
    rect.translate(-(rf.width() - 2 * radius) - 1.0, 0.0);
    path.arcTo(rect, -90.0, -90.0);
    path.lineTo(r.left(), r.top() + radius + 1);

    p.drawPath(path);
}

namespace DragSupport
{

QPixmap makePixmap(const QString &text, const QFont &font)
{
    QColor background(234, 178, 230);
    QFontMetrics fm(font);

    int height = 2 * fm.height();

    QSize bonusSize (height, 0);
    QSize size(fm.width(text), height);

    // The next 3 lines allow us to programmatically construct a QPixmap with
    // a completely transparent background.

    QImage image(size + bonusSize, QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::transparent);

    QPixmap pix = QPixmap::fromImage(image);

    { // QPainter will destruct automatically
	QPainter painter(&pix);
	painter.setFont(font);

	painter.setBrush(background);
	painter.setPen(Qt::black);

	// XCURSOR doesn't support anti-aliased pixmaps or something? Either
	// way it doesn't really look that great.
	painter.setRenderHint(QPainter::Antialiasing, false);

	QRect r = image.rect();
	r.setRight(r.right() - 1);
	r.setBottom(r.bottom() - 1);

	drawCircularRoundRect(painter, r, 9);
	// roundRect is annoying in that the four "pies" in each corner aren't
	// circular, they're elliptical.  Try to make the radii force it circular
	// again.
//	painter.drawRoundRect(r, 75 * image.height() / image.width(), 75);

	// Alias better names for some constants.
	int textLeft = height / 2;

	// Draw text
	painter.setPen(Qt::black);
	painter.drawText(textLeft, height / 4, size.width(), size.height(), 0, text);
    } // QPainter is gone

    return pix;
}

} // DragSupport
