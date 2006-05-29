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

#include "evaluator.h"
#include "function.h"
#include "lexer.h"
#include "node.h" // For parser_yacc.hpp below
#include "parser_yacc.hpp"

//
// Reimplementation of goodies from Evaluator follows.
//

Evaluator::Evaluator()
{
}

QString Evaluator::autoFix(const QString& expr)
{
    int par = 0;
    QString result;

    // strip off all funny characters
    for(int c = 0; c < expr.length(); ++c)
        if(expr.at(c).isPrint())
            result.append(expr[c]);

    TokenList tokens = Lexer::tokenize(result);

    // automagically close all parenthesis
    foreach(Token token, tokens) {
        if(token.asOperator() == Token::LeftPar)
            par++;
        else if(token.asOperator() == Token::RightPar)
            par--;
    }

    for(; par > 0; par--)
        result.append(')');

    // special treatment for simple function
    // e.g. "cos" is regarded as "cos(ans)"
    if(tokens.count() == 1 && FunctionManager::instance()->isFunction(tokens[0].text()))
        result.append("(ans)");

    return result;
}

// vim: set et ts=8 sw=4:
