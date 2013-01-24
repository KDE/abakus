/*
 * function.cpp - part of abakus
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
#include "function.h"
#include "node.h"
#include "numeralmodel.h"

// Applies the function identified by func, using value as a parameter.
Abakus::Number evaluateFunction(const Function *func, const Abakus::Number value)
{
    if(func->userDefined) {
        // Pull real entry from userFunctionTable
        UserFunction *realFunction = func->userFn;

        bool wasSet = NumeralModel::instance()->isValueSet(realFunction->varName);
        Abakus::Number oldValue;
        if(wasSet)
            oldValue = NumeralModel::instance()->value(realFunction->varName);

        NumeralModel::instance()->setValue(realFunction->varName, value);
        Abakus::Number result = realFunction->fn->value();

        if(wasSet)
            NumeralModel::instance()->setValue(realFunction->varName, oldValue);
        else
            NumeralModel::instance()->removeValue(realFunction->varName);

        return result;
    }

    return (value.*(func->fn))();
}

void setTrigMode(Abakus::TrigMode mode)
{
    Abakus::m_trigMode = mode;
}

Abakus::TrigMode trigMode()
{
    return Abakus::m_trigMode;
}

#include "function.moc"

// vim: set et sw=4 ts=8:
