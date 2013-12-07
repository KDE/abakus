/*
 * main.cpp - part of abakus
 * Copyright (C) 2013 Mathias Kraus <k.hias@gmx.de>
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

#include "mainobject.h"

#include <sailfishapp.h>

#include <QGuiApplication>
#include <QQuickView>

#include <mpfr.h>

const char *const version = "0.92";

int main(int argc, char **argv)
{
    mpfr_set_default_prec(6 * 78); // 78 digits, figure about 6 bits needed.

    QScopedPointer<QGuiApplication> app(SailfishApp::application(argc, argv));
    QScopedPointer<MainObject> mainObject(new MainObject());
    QScopedPointer<QQuickView> view(SailfishApp::createView());

    mainObject->setView(view.data());
    view->setSource(SailfishApp::pathTo("qml/main.qml"));
    view->show();

    return app->exec();
}
