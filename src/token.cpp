/*
 * token.cpp - part of abakus
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
#include "token.h"
#include "node.h"
#include "parser_yacc.hpp"

#include <QList>

Token::Token(int lexType, const QString &text, int pos) :
    m_token(lexType), m_type(Unknown), m_text(text), m_pos(pos)
{
    switch(lexType)
    {
	case NUM:
	    m_type = Number;
	    break;

	case FN:
	case ID:
	case SET:
	case REMOVE:
	case DERIV:
	    m_type = Identifier;
	    break;

	case POWER:
        case '+':
        case '-':
        case '*':
        case '/':
        case ',':
        case '(':
        case ')':
        case '%':
        case '=':
	    m_type = Operator;
	    break;

	default:
	    break;
    }
}

Token::Token(const Token &other) : m_token(other.token()),
				   m_type(other.type()), m_text(other.text()),
				   m_pos(other.pos())
{
}

Token &Token::operator=(const Token &other)
{
    if(this == &other)
	return *this;

    m_token = other.token();
    m_type = other.type();
    m_text = other.text();
    m_pos = other.pos();

    return *this;
}

Token::Op Token::asOperator() const
{
    if(text().length() == 1) {
	switch(text().at(0).unicode())
	{
	    case '+': return Plus;
	    case '-': return Minus;
	    case '*': return Asterisk;
	    case '/': return Slash;
	    case '^': return Caret;
	    case ',': return Comma;
	    case '(': return LeftPar;
	    case ')': return RightPar;
	    case '%': return Percent;
	    case '=': return Equal;
	    default : return InvalidOp;
	}
    }
    else if(text() == QLatin1String("**"))
	return Caret;

    return InvalidOp;
}

