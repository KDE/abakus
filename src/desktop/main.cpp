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

#ifdef ABAKUS_QTONLY
    #include <QGuiApplication>
#else
    #include <KAboutData>
    #include <KApplication>
    #include <KCmdLineArgs>
    #include <KLocale>
#endif

#include <mpfr.h>

const char *const version = "0.92";

int main(int argc, char **argv)
{
    mpfr_set_default_prec(6 * 78); // 78 digits, figure about 6 bits needed.
#ifdef ABAKUS_QTONLY
    QGuiApplication app(argc, argv);
#else
    KAboutData *about = new KAboutData(
        "abakus", // appName
        "abakus", // i18n catalog
        ki18n("abakus"), // User-visible name
        version,
        ki18n("A simple keyboard-driven calculator"),
        KAboutData::License_GPL_V2,
        ki18n("Copyright (c) 2013 Mathias Kraus\nCopyright (c) 2004, 2005, 2007 Michael Pyne"));

    about->addAuthor(ki18n("Mathias Kraus"),
                     ki18n("Developer"),
                     "k.hias@gmx.de",
                     0);
    about->addAuthor(ki18n("Michael Pyne"),
                     ki18n("Developer"),
                     "mpyne@kde.org",
                     "http://purinchu.net/");
    about->addCredit(ki18n("Ariya Hidayat"),
                     ki18n("High precision math routines, and inspiration for the new design came from his C++ implementation (SpeedCrunch)"),
                     "ariya@kde.org",
                     "http://speedcrunch.berlios.de/");
    about->addCredit(ki18n("Roberto Alsina"),
                     ki18n("Came up with the initial idea, along with a Python implementation."),
                     "ralsina@netline.com.ar",
                     "http://dot.kde.org/1096309744/");
    about->addCredit(ki18n("Zack Rusin"),
                     ki18n("Inspiration/code for the initial design came from his Ruby implementation."),
                     "zack@kde.org");

    KCmdLineArgs::init(argc, argv, about);
    KApplication app;
#endif
    MainWindow *win = new MainWindow;
    win->show();

    return app.exec();
}

// vim: set et sw=4 ts=8:
