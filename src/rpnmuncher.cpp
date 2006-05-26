/*
 * rpnmuncher.cpp - part of abakus
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
//#include <math.h>

#include <kdebug.h>
#include <klocalizedstring.h>

#include <QStack>
#include <QString>

#include "numerictypes.h"
#include "node.h" // For parser_yacc.hpp
#include "parser_yacc.hpp"
#include "lexer.h"
#include "rpnmuncher.h"
#include "valuemanager.h"
#include "function.h"

/**
 * Holds either a textual identifier, or a numeric value.
 */
class Operand
{
    public:
    Operand() : m_isValue(true), m_value(0) { }
    Operand(const QString &ident) : m_isValue(false), m_text(ident) { }
    Operand(Abakus::number_t value) : m_isValue(true), m_value(value) { }

    Abakus::number_t value() const
    {
        if(m_isValue)
            return m_value;

        return ValueManager::instance()->value(m_text);
    }

    operator Abakus::number_t() const
    {
        return value();
    }

    QString text() const { return m_text; }

    private:
    bool m_isValue;
    QString m_text;
    Abakus::number_t m_value;
};

QString RPNParser::m_errorStr;
bool RPNParser::m_error(false);
OperandStack RPNParser::m_stack;

struct Counter
{
    ~Counter() {
        Abakus::number_t count( static_cast<int>(RPNParser::stack().count()) );
        ValueManager::instance()->setValue("stackCount", count);
    }
};

Abakus::number_t RPNParser::rpnParseString(const QString &text)
{
    TokenList tokens = Lexer::tokenize(text);
    Counter counter; // Will update stack count when we leave proc.
    (void) counter;  // Avoid warnings about it being unused.

    // Used in the case statements below
    Operand l, r;
    FunctionManager *manager = FunctionManager::instance();
    Function *fn = 0;

    m_error = false;
    m_errorStr = QString::null;

    foreach(Token token, tokens) {
        switch(token.token())
        {
        case NUM:
            m_stack.push(Abakus::number_t(token.text()));
        break;

        case ID:
            if(token.text().toLower() == QLatin1String("pop")) {
                if(m_stack.isEmpty()) {
                    m_error = true;
                    m_errorStr = i18n("Can't pop from an empty stack.");
                    return Abakus::number_t::nan();
                }

                m_stack.pop();
            }
            else if(token.text().toLower() == QLatin1String("clear"))
                m_stack.clear();
            else
                m_stack.push(token.text());
        break;

        case FN:
            if(m_stack.count() < 1) {
                m_error = true;
                m_errorStr = i18n("Insufficient operands for function %1", token.text());
                return Abakus::number_t::nan();
            }

            fn = manager->function(token.text());

            l = m_stack.pop();
            m_stack.push(evaluateFunction(fn, l));
        break;

        case SET:
        case REMOVE:
            m_error = true;
            m_errorStr = i18n("The set and remove commands can only be used in normal mode.");
            return Abakus::number_t::nan();
        break;

        case POWER:
            if(m_stack.count() < 2) {
                m_error = true;
                m_errorStr = i18n("Insufficient operands for exponentiation operator.");
                return Abakus::number_t::nan();
            }

            r = m_stack.pop();
            l = m_stack.pop();
            m_stack.push(l.value().pow(r));
        break;

        case '=':
            r = m_stack.pop();
            l = m_stack.pop();
            ValueManager::instance()->setValue(l.text(), r);

            m_stack.push(l);
        break;

        case '+':
            if(m_stack.count() < 2) {
                m_error = true;
                m_errorStr = i18n("Insufficient operands for addition operator.");
                return Abakus::number_t::nan();
            }

            r = m_stack.pop();
            l = m_stack.pop();
            m_stack.push(l.value() + r.value());
        break;

        case '-':
            if(m_stack.count() < 2) {
                m_error = true;
                m_errorStr = i18n("Insufficient operands for subtraction operator.");
                return Abakus::number_t::nan();
            }

            r = m_stack.pop();
            l = m_stack.pop();
            m_stack.push(l.value() - r.value());
        break;

        case '*':
            if(m_stack.count() < 2) {
                m_error = true;
                m_errorStr = i18n("Insufficient operands for multiplication operator.");
                return Abakus::number_t::nan();
            }

            r = m_stack.pop();
            l = m_stack.pop();
            m_stack.push(l.value() * r.value());
        break;

        case '/':
            if(m_stack.count() < 2) {
                m_error = true;
                m_errorStr = i18n("Insufficient operands for division operator.");
                return Abakus::number_t::nan();
            }

            r = m_stack.pop();
            l = m_stack.pop();
            m_stack.push(l.value() / r.value());
        break;

        default:
            // Impossible case happened.
            kError() << "Impossible case happened in " << k_funcinfo << endl;
            kError() << "Token is " << token.text() << endl;
            kError() << "Token type is " << token.token() << endl;
            m_error = true;
            m_errorStr = "Bug found in program, please report.";
            return Abakus::number_t::nan();
        }
    }

    // TODO: Should this be an error?
    if(m_stack.isEmpty())
        return Abakus::number_t::nan();

    return m_stack.pop();
}

// vim: set et sw=4 ts=8:
