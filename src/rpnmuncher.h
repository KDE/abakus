#ifndef ABAKUS_RPNMUNCHER_H
#define ABAKUS_RPNMUNCHER_H
/*
 * rpnmuncher.h - part of abakus
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

class QString;
class Operand;

template<class T> class QStack;
typedef QStack<Operand> OperandStack;

#include "numerictypes.h"

class RPNParser
{
    public:
    static Abakus::number_t rpnParseString(const QString &text);
    static bool wasError() { return m_error; }
    static QString errorString() { return m_errorStr; }
    static OperandStack &stack() { return m_stack; }

    private:
    static QString m_errorStr;
    static bool m_error;
    static OperandStack m_stack;
};

#endif
