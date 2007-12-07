/*
 * dragsupport.cpp - part of abakus
 * Copyright (C) 2004, 2005 Michael Pyne <michael.pyne@kdemail.net>
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

#include <QtCore/QString>
#include <QtGui/QPixmap>
#include <QtGui/QImage>
#include <QtGui/QPainter>
#include <QtGui/QColor>
#include <QtGui/QFont>
#include <QtGui/QBrush>
#include <QtGui/QFontMetrics>

#include "dragsupport.h"

namespace DragSupport
{

QPixmap makePixmap(const QString &text, const QFont &font)
{
    QColor background(234, 178, 230);
    QFontMetrics fm(font);

    int height = 2 * fm.height();
    QSize bonusSize (height, 0);
    QSize size(fm.width(text), height);
    QImage image(size + bonusSize, QImage::Format_ARGB32_Premultiplied);

    QPainter painter(&image);
    painter.setFont(font);

    // Outline black, background white
    painter.setPen(Qt::black);
    painter.setBrush(background);

    // roundRect is annoying in that the four "pies" in each corner aren't
    // circular, they're elliptical.  Try to make the radii force it circular
    // again.
    painter.drawRoundRect(image.rect(), 75 * image.height() / image.width(), 75);

    // Alias better names for some constants.
    int textLeft = height / 2;

    // Draw text
    painter.setPen(Qt::black);
    painter.drawText(textLeft, height / 4, size.width(), size.height(), 0, text);

    return QPixmap::fromImage(image);
}

} // DragSupport

// vim: set et sw=4 ts=8:
