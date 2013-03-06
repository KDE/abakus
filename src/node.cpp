/*
 * node.cpp - part of abakus
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

#include "functionmodel.h"
#include "node.h"
#include "numeralmodel.h"

#include <QDebug>

void Node::deleteNode(Node *node)
{
    if(dynamic_cast<BaseFunction *>(node) != 0)
        delete node;
}

BaseFunction::BaseFunction(const char *name) :
    m_name(name)
{
}

const Function *BaseFunction::function() const
{
    return FunctionModel::instance()->function(m_name);
}

UnaryFunction::UnaryFunction(const char *name, Node *operand) :
    BaseFunction(name), m_node(operand)
{
}

UnaryFunction::~UnaryFunction()
{
    deleteNode(m_node);
    m_node = 0;
}

void UnaryFunction::setOperand(Node *operand)
{
    m_node = operand;
}

void UnaryFunction::applyMap(NodeFunctor &fn) const
{
    fn(operand());
    fn(this);
}

QString UnaryFunction::infixString() const
{
    return QString("%1(%2)").arg(name(), operand()->infixString());
}

BuiltinFunction::BuiltinFunction(const char *name, Node *operand) :
    UnaryFunction(name, operand)
{
}

Abakus::Number BuiltinFunction::value() const
{
    if(function() && operand()) {
        Abakus::Number fnValue = operand()->value();
        return evaluateFunction(function(), fnValue);
    }

    return Abakus::Number(0);
}

Abakus::Number BuiltinFunction::derivative() const
{
    Abakus::Number du = operand()->derivative();
    Abakus::Number value = operand()->value();
    Abakus::Number one(1), zero(0);

    if(du == zero)
        return du;

    // In case these functions get added later, these derivatives may
    // be useful:
    // d/dx(asinh u) = (du/dx * 1 / sqrt(x^2 + 1))
    // d/dx(acosh u) = (du/dx * 1 / sqrt(x^2 - 1))
    // d/dx(atanh u) = (du/dx * 1 / (1 - x^2))

    // This is very unfortunate duplication.
    if(name() == "sin")
        return value.cos() * du;
    else if(name() == "cos")
        return -value.sin() * du;
    else if(name() == "tan") {
        Abakus::Number cosResult;

        cosResult = value.cos();
        cosResult = cosResult * cosResult;
        return one / cosResult;
    }
    else if(name() == "asinh") {
        value = value * value + one;
        return du / value.sqrt();
    }
    else if(name() == "acosh") {
        value = value * value - one;
        return du / value.sqrt();
    }
    else if(name() == "atanh") {
        value = one - value * value;
        return du / value;
    }
    else if(name() == "sinh") {
        return du * value.cosh();
    }
    else if(name() == "cosh") {
        return du * value.sinh(); // Yes the sign is correct.
    }
    else if(name() == "tanh") {
        Abakus::Number tanh = value.tanh();

        return du * (one - tanh * tanh);
    }
    else if(name() == "atan") {
        return one * du / (one + value * value);
    }
    else if(name() == "acos") {
        // Same as asin but with inverted sign.
        return -(one / (value * value - one).sqrt() * du);
    }
    else if(name() == "asin") {
        return one / (value * value - one).sqrt() * du;
    }
    else if(name() == "ln") {
        return du / value;
    }
    else if(name() == "exp") {
        return du * value.exp();
    }
    else if(name() == "log") {
        return du / value / Abakus::Number(10).ln();
    }
    else if(name() == "sqrt") {
        Abakus::Number half("0.5");
        return half * value.pow(-half) * du;
    }
    else if(name() == "abs") {
        return (value / value.abs()) * du;
    }

    // Approximate it.

    Abakus::Number epsilon("1e-15");
    Abakus::Number fxh = evaluateFunction(function(), value + epsilon);
    Abakus::Number fx = evaluateFunction(function(), value);
    return (fxh - fx) / epsilon;
}

DerivativeFunction::~DerivativeFunction()
{
    deleteNode(m_operand);
    m_operand = 0;
}

Abakus::Number DerivativeFunction::value() const
{
    NumeralModel *vm = NumeralModel::instance();
    Abakus::Number result;

    if(vm->isValueSet("x")) {
        Abakus::Number oldValue = vm->value("x");

        vm->setValue("x", m_where->value());
        result = m_operand->derivative();
        vm->setValue("x", oldValue);
    }
    else {
        vm->setValue("x", m_where->value());
        result = m_operand->derivative();
        vm->removeValue("x");
    }

    return result;
}

Abakus::Number DerivativeFunction::derivative() const
{
    qWarning() << endl;
    qWarning() << "This function is never supposed to be called!\n";

    return m_operand->derivative();
}

void DerivativeFunction::applyMap(NodeFunctor &fn) const
{
    fn(m_operand);
    fn(this);
}

QString DerivativeFunction::infixString() const
{
    return QString("deriv(%1, %2)").arg(m_operand->infixString(), m_where->infixString());
}

UnaryOperator::UnaryOperator(Type type, Node *operand)
    : m_type(type), m_node(operand)
{
}

UnaryOperator::~UnaryOperator()
{
    deleteNode(m_node);
    m_node = 0;
}

void UnaryOperator::applyMap(NodeFunctor &fn) const
{
    fn(operand());
    fn(this);
}

QString UnaryOperator::infixString() const
{
    if(dynamic_cast<BinaryOperator *>(operand()))
        return QString("-(%1)").arg(operand()->infixString());

    return QString("-%1").arg(operand()->infixString());
}

Abakus::Number UnaryOperator::derivative() const
{
    switch(type()) {
        case Negation:
            return -(operand()->derivative());

        default:
            qWarning() << "Impossible case encountered for UnaryOperator!\n";
            return Abakus::Number(0);
    }
}

Abakus::Number UnaryOperator::value() const
{
    switch(type()) {
        case Negation:
            return -(operand()->value());

        default:
            qWarning() << "Impossible case encountered for UnaryOperator!\n";
            return Abakus::Number(0);
    }
}

BinaryOperator::BinaryOperator(Type type, Node *left, Node *right) :
    m_type(type), m_left(left), m_right(right)
{
}

BinaryOperator::~BinaryOperator()
{
    deleteNode(m_left);
    m_left = 0;

    deleteNode(m_right);
    m_right = 0;
}

void BinaryOperator::applyMap(NodeFunctor &fn) const
{
    fn(leftNode());
    fn(rightNode());
    fn(this);
}

QString BinaryOperator::infixString() const
{
    QString op;

    switch(type()) {
        case Addition:
            op = "+";
        break;

        case Subtraction:
            op = "-";
        break;

        case Multiplication:
            op = "*";
        break;

        case Division:
            op = "/";
        break;

        case Exponentiation:
            op = "^";
        break;

        default:
            op = "Error";
    }
    
    QString left = QString(isSimpleNode(leftNode()) ? "%1" : "(%1)").arg(leftNode()->infixString());
    QString right = QString(isSimpleNode(rightNode()) ? "%1" : "(%1)").arg(rightNode()->infixString());

    return QString("%1 %2 %3").arg(left, op, right);
}

Abakus::Number BinaryOperator::derivative() const
{
    if(!leftNode() || !rightNode()) {
        qWarning() << "Can't evaluate binary operator!\n";
        return Abakus::Number(0);
    }

    Abakus::Number f = leftNode()->value();
    Abakus::Number fPrime = leftNode()->derivative();
    Abakus::Number g = rightNode()->value();
    Abakus::Number gPrime = rightNode()->derivative();

    switch(type()) {
        case Addition:
            return fPrime + gPrime;

        case Subtraction:
            return fPrime - gPrime;

        case Multiplication:
            return f * gPrime + fPrime * g;

        case Division:
            return (g * fPrime - f * gPrime) / (g * g);

        case Exponentiation:
            return f.pow(g) * ((g / f) * fPrime + gPrime * f.ln());

        default:
            qWarning() << "Impossible case encountered evaluating binary operator!\n";
            return Abakus::Number(0);
    }
}

Abakus::Number BinaryOperator::value() const
{
    if(!leftNode() || !rightNode()) {
        qWarning() << "Can't evaluate binary operator!\n";
        return Abakus::Number(0);
    }

    Abakus::Number lValue = leftNode()->value();
    Abakus::Number rValue = rightNode()->value();

    switch(type()) {
        case Addition:
            return lValue + rValue;

        case Subtraction:
            return lValue - rValue;

        case Multiplication:
            return lValue * rValue;

        case Division:
            return lValue / rValue;

        case Exponentiation:
            return lValue.pow(rValue);
            
        case LogicalShiftLeft:
            return lValue * Abakus::Number(2).pow(rValue);
            
        case LogicalShiftRight:
            return Abakus::Number(lValue / Abakus::Number(2).pow(rValue)).trunc();

        default:
            qWarning() << "Impossible case encountered evaluating binary operator!\n";
            return Abakus::Number(0);
    }
}

bool BinaryOperator::isSimpleNode(Node *node) const
{
    if(dynamic_cast<Identifier *>(node) ||
       dynamic_cast<NumericValue *>(node) ||
       dynamic_cast<UnaryOperator *>(node) ||
       dynamic_cast<BaseFunction *>(node))
    {
        return true;
    }

    return false;
}

Identifier::Identifier(const char *name) : m_name(name)
{
}

Abakus::Number Identifier::value() const
{
    return NumeralModel::instance()->value(name());
}

void Identifier::applyMap(NodeFunctor &fn) const
{
    fn(this);
}

QString NumericValue::infixString() const
{
    return value().toString();
}

// vim: set et ts=8 sw=4:
