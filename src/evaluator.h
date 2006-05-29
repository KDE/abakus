/* This file was part of the SpeedCrunch project
   Copyright (C) 2004 Ariya Hidayat <ariya@kde.org>

   And is now part of abakus.
   Copyright (c) 2005, 2006 Michael Pyne <michael.pyne@kdemail.net>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef ABAKUS_EVALUATOR_H
#define ABAKUS_EVALUATOR_H

class QString;

class Evaluator
{
public:
    Evaluator();

    static QString autoFix( const QString& expr );

private:
    Evaluator( const Evaluator& );
    Evaluator& operator=( const Evaluator& );
};


#endif // EVALUATOR

// vim: set et ts=8 sw=4:
