#ifndef ABAKUS_TOKEN_H
#define ABAKUS_TOKEN_H
/*
 * token.h - part of abakus
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

class Token
{
public:
    typedef enum
    {
	Unknown,
	Number,
	Operator,
	Identifier
    } Type;

    typedef enum
    {
	InvalidOp = 0,
	Plus,           //  + (addition)
	Minus,          //  - (substraction, negation)
	Asterisk,       //  * (multiplication)
	Slash,          //  / (division)
	Caret,          //  ^ (power) or **.
	LeftPar,        //  (
	RightPar,       //  )
	Comma,          // argument separator
	Percent,
	Equal           // variable assignment
    } Op;

    Token(int lexType = 0, const QString& text = QString(), int pos = -1);

    Token(const Token &);
    Token & operator=(const Token &);

    /// Return the exact symbol (e.g. SET, DERIV, PLUS)
    int token() const { return m_token; }

    /// Return the symbol type (e.g. Identifier, Number)
    Type type() const { return m_type; }

    QString text() const { return m_text; }
    int pos() const { return m_pos; };

    Op asOperator() const;

private:
    int m_token;
    Type m_type;
    QString m_text;
    int m_pos;
};

template<class T>
class QList;

typedef QList<Token> TokenList;

#endif
