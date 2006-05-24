#ifndef ABAKUS_LEXER_H
#define ABAKUS_LEXER_H
/*
 * lexer.h - part of abakus
 * Copyright (C) 2006 Michael Pyne <michael.pyne@kdemail.net>
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

// Predeclarations.

#include <QString>
#include <QList>
#include "token.h"

// Implemented in lexer.l due to prototype issues.
Abakus::number_t parseString(const char *str);

// Implemented in lexer.l due to prototype issues.

/**
 * This class is used to split input into conceptual tokens to use parsing
 * later.
 */
class Lexer
{
public:
    Lexer(const QString &expr);
    ~Lexer();

    bool hasNext() const;

    // Advances the internal iterator, and returns the token type.  Use
    // tokenValue() to determine the actual text of the token.  A return value
    // of 0 indicates that the end of input has been reached.
    int nextType();

    int tokenPos() const;

    // Can call this after nextType to find the associated string value of the
    // token.
    QString tokenValue() const;

    static TokenList tokenize(const QString &expr);

private:
    class Private;
    Private *m_private;
};

#endif
