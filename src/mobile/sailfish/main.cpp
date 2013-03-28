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

#include "mainwindow.h"

#include <QApplication>
#include <QDeclarativeView>
#include "sailfishapplication.h"

#include <mpfr.h>

const char *const version = "0.92";

int main(int argc, char **argv)
{
    mpfr_set_default_prec(6 * 78); // 78 digits, figure about 6 bits needed.

    QScopedPointer<QApplication> app(Sailfish::createApplication(argc, argv));
    QScopedPointer<QDeclarativeView> view(Sailfish::createView("main.qml"));

    MainWindow mainObject(view.data());

    view->setSource(QUrl::fromLocalFile(QString(DEPLOYMENT_PATH) + "main.qml"));

    Sailfish::showView(view.data());

    return app->exec();
}

// vim: set et sw=4 ts=8:
