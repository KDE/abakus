/*
 * abakus.cpp - part of abakus
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
#include <kaboutdata.h>
#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kdebug.h>

#include <config.h>

#if HAVE_MPFR
#include <mpfr.h>
#endif

#include "mainwindow.h"

const char *const version = "0.90";

int main(int argc, char **argv)
{
    KAboutData *about = new KAboutData("abakus", I18N_NOOP("abakus"), version,
        I18N_NOOP("A simple keyboard-driven calculator"), KAboutData::License_GPL,
	"(c) 2004, 2005 Michael Pyne", 0 /* text */, "http://grammarian.homelinux.net/abakus/",
	"michael.pyne@kdemail.net");

    about->addAuthor("Michael Pyne",
                     I18N_NOOP("Developer"),
                     "michael.pyne@kdemail.net",
		     "http://grammarian.homelinux.net/");
    about->addCredit("Ariya Hidayat",
                     I18N_NOOP("High precision math routines, and inspiration for the new design came from his C++ implementation (SpeedCrunch)"),
		     "ariya@kde.org",
		     "http://speedcrunch.berlios.de/");
    about->addCredit("Roberto Alsina",
                     I18N_NOOP("Came up with the initial idea, along with a Python implementation."),
		     "ralsina@netline.com.ar",
		     "http://dot.kde.org/1096309744/");
    about->addCredit("Zack Rusin",
                     I18N_NOOP("Inspiration/code for the initial design came from his Ruby implementation."),
		     "zack@kde.org");

#if HAVE_MPFR
    mpfr_set_default_prec(6 * 78); // 78 digits, figure about 6 bits needed.
    kdDebug() << "Using the MPFR high-precision library.\n";
#else
    kdDebug() << "Using the internal high-precision library.\n";
#endif

    KCmdLineArgs::init(argc, argv, about);
    KApplication app;
    MainWindow *win = new MainWindow;

    app.setMainWidget(win);
    app.connect(&app, SIGNAL(lastWindowClosed()), SLOT(quit()));
    win->show();
    win->resize(500, 300);
    
    return app.exec();
}
