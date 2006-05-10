#ifndef ABAKUS_DCOP_IFACE
#define ABAKUS_DCOP_IFACE
/*
 * dcopIface.h - part of abakus
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

#include <kdebug.h>
#include <dcopobject.h>

#include <qstring.h>
#include <kvbox.h>

#include "mainwindow.h"
#include "numerictypes.h"
#include "function.h"

class AbakusIface : virtual public DCOPObject
{
    K_DCOP
    public:
    AbakusIface() : DCOPObject("Calculator")
    {
    }

    k_dcop:
    virtual double evaluate(const QString &expr)
    {
	Abakus::number_t result = parseString(expr.toLatin1());
	return result.asDouble();
    }
};

#endif
