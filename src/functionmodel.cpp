/*
 * functionmodel.cpp - part of abakus
 * Copyright (C) 2012 Mathias Kraus <k.hias@gmx.net>
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
#include "numerictypes.h"

#include <klocale.h>
#include <kdebug.h>

#include <QtCore/QVector>
#include <QtCore/QString>
#include <QtCore/QRegExp>

#include <math.h>

#include "functionmodel.h"
#include "node.h"
#include "numeralmodel.h"
#include "hmath.h"

// Used to try and avoid recursive function definitions
class DupFinder : public NodeFunctor
{
    public:
    DupFinder(const QString &nameToFind) :
        m_name(nameToFind), m_valid(true)
    {
    }

    virtual ~DupFinder() { }

    bool isValid() const { return m_valid; }

    virtual void operator()(const Node *node)
    {
        if(!m_valid)
            return;

        const BaseFunction *fn = dynamic_cast<const BaseFunction *>(node);
        if(fn && fn->name() == m_name)
            m_valid = false; // Duplicate detected
    }

    private:
    QString m_name;
    bool m_valid;
};

// Define static member for FunctionManager
FunctionModel *FunctionModel::m_manager = 0;

FunctionModel *FunctionModel::instance()
{
    if(!m_manager)
        m_manager = new FunctionModel;

    return m_manager;
}

FunctionModel::FunctionModel(QObject *parent) :
    QAbstractListModel(parent)
{
    setObjectName("FunctionManager");
    
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[VariableRole] = "variable";
    roles[ValueRole] = "value";
    roles[DescriptionRole] = "description";
    roles[TypeStringRole] = "typeString";
    setRoleNames(roles);
    
    addBuitInFunctions();
}

FunctionModel::~FunctionModel()
{
    beginRemoveRows(QModelIndex(), 0, rowCount()-1);
    while(!m_functionModelItems.isEmpty())
    {
        delete m_functionModelItems.takeFirst();
    }
    endRemoveRows();
}

int FunctionModel::functionModelItemIndex(const QString& name) const
{
    for(int i = 0; i < m_functionModelItems.count(); ++i)
    {
        if(m_functionModelItems[i]->name() == name)
        {
            return i;
        }
    }
    
    return -1;
}

int FunctionModel::functionModelItemIndex(const FunctionModelItem::FunctionItemType &type) const
{
    for(int i = 0; i < m_functionModelItems.count(); ++i)
    {
        if(m_functionModelItems[i]->type() == type)
        {
            return i;
        }
    }
    
    return -1;
}

int FunctionModel::rowCount(const QModelIndex & parent) const
{
    return m_functionModelItems.count();
}

QVariant FunctionModel::data(const QModelIndex & index, int role) const
{
    if (index.row() < 0 || index.row() > m_functionModelItems.count())
    {
        return QVariant();
    }
    
    FunctionModelItem *functionModelItem = m_functionModelItems[index.row()];
    if (role == NameRole)
    {
        return functionModelItem->name();
    }
    else if (role == VariableRole)
    {
        return functionModelItem->variable();
    }
    else if (role == ValueRole)
    {
        return functionModelItem->value();
    }
    else if (role == DescriptionRole)
    {
        return functionModelItem->description();
    }
    else if (role == TypeStringRole)
    {
        return functionModelItem->typeString();
    }
    return QVariant();
}

// Dummy return value to enable static initialization in the DECL_*()
// macros.
bool FunctionModel::addFunction(const QString &name, function_t fn, const QString &desc)
{
    Function *newFn = new Function;
    QRegExp returnTrigRE("^a(cos|sin|tan)");
    QRegExp needsTrigRE("^(cos|sin|tan)");
    QString fnName(name);

    newFn->name = name;
    newFn->description = desc;
    newFn->fn = fn;
    newFn->userDefined = false;
    newFn->returnsTrig = fnName.contains(returnTrigRE);
    newFn->needsTrig = fnName.contains(needsTrigRE);

    FunctionModelItem* functionModelItem = new FunctionModelItem(newFn, FunctionModelItem::BuiltInFunction);
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_functionModelItems << functionModelItem;
    endInsertRows();

    return false;
}

void FunctionModel::addBuitInFunctions()
{
    addFunction("sin", &Abakus::number_t::sin, i18n("Trigonometric sine"));
    addFunction("cos", &Abakus::number_t::cos, i18n("Trigonometric cosine"));
    addFunction("tan", &Abakus::number_t::tan, i18n("Trigonometric tangent"));
    
    addFunction("sinh", &Abakus::number_t::sinh, i18n("Hyperbolic sine"));
    addFunction("cosh", &Abakus::number_t::cosh, i18n("Hyperbolic cosine"));
    addFunction("tanh", &Abakus::number_t::tanh, i18n("Hyperbolic tangent"));
    
    addFunction("atan", &Abakus::number_t::atan, i18n("Inverse tangent"));
    addFunction("acos", &Abakus::number_t::acos, i18n("Inverse cosine"));
    addFunction("asin", &Abakus::number_t::asin, i18n("Inverse sine"));
    
    addFunction("asinh", &Abakus::number_t::asinh, i18n("Inverse hyperbolic sine"));
    addFunction("acosh", &Abakus::number_t::acosh, i18n("Inverse hyperbolic cosine"));
    addFunction("atanh", &Abakus::number_t::atanh, i18n("Inverse hyperbolic tangent"));
    
    addFunction("abs",  &Abakus::number_t::abs,  i18n("Absolute value of number"));
    addFunction("sqrt", &Abakus::number_t::sqrt, i18n("Square root"));
    addFunction("ln",   &Abakus::number_t::ln,   i18n("Natural logarithm (base e)"));
    addFunction("log",  &Abakus::number_t::log,  i18n("Logarithm (base 10)"));
    addFunction("exp",  &Abakus::number_t::exp,  i18n("Natural exponential function"));
    
    addFunction("round", &Abakus::number_t::round,   i18n("Round to nearest number"));
    addFunction("ceil",  &Abakus::number_t::ceil,    i18n("Nearest greatest integer"));
    addFunction("floor", &Abakus::number_t::floor,   i18n("Nearest lesser integer"));
    addFunction("int",   &Abakus::number_t::integer, i18n("Integral part of number"));
    addFunction("frac",  &Abakus::number_t::frac,    i18n("Fractional part of number"));
}

Function *FunctionModel::function(const QString &name) const
{
    int functionIndex = functionModelItemIndex(name);
    if(functionIndex != -1)
    {
        return m_functionModelItems[functionIndex]->function();
    }

    return 0;
}

// Returns true if the named identifier is a function, false otherwise.
bool FunctionModel::isFunction(const QString &name)
{
    return functionModelItemIndex(name) != -1;
}

bool FunctionModel::isFunctionUserDefined(const QString &name)
{
    int functionIndex = functionModelItemIndex(name);
    return (functionIndex != -1) && (m_functionModelItems[functionIndex]->type() == FunctionModelItem::UserFunction);
}

bool FunctionModel::addFunction(BaseFunction *fn, const QString &dependantVar)
{
    // First see if this function is recursive
    DupFinder dupFinder(fn->name());
    UnaryFunction *unFunction = dynamic_cast<UnaryFunction *>(fn);
    if(unFunction && unFunction->operand()) {
        unFunction->operand()->applyMap(dupFinder);
        if(!dupFinder.isValid())
            return false;
    }

    // Structure holds extra data needed to call the user defined
    // function.
    UserFunction *newFn = new UserFunction;
    newFn->fn = fn;
    newFn->varName = QString(dependantVar);

    // Now setup the Function data structure that holds the information
    // we need to access and call the function later.
    Function *fnTabEntry = new Function;
    fnTabEntry->name = fn->name();
    fnTabEntry->userFn = newFn;
    fnTabEntry->returnsTrig = false;
    fnTabEntry->needsTrig = false;
    fnTabEntry->userDefined = true;

    int functionIndex = functionModelItemIndex(fnTabEntry->name);
    if(functionIndex != -1)
    {
        m_functionModelItems[functionIndex]->setFunction(fnTabEntry);
        emit dataChanged(index(functionIndex), index(functionIndex));
    }
    else if(functionIndex == -1)
    {
        FunctionModelItem* functionModelItem = new FunctionModelItem(fnTabEntry, FunctionModelItem::UserFunction);
        int functionIndexOfFirstBuiltInFunction = functionModelItemIndex(FunctionModelItem::BuiltInFunction);
        if(functionIndexOfFirstBuiltInFunction == -1)
        {
            functionIndexOfFirstBuiltInFunction = rowCount();
        }
        
        beginInsertRows(QModelIndex(), functionIndexOfFirstBuiltInFunction, functionIndexOfFirstBuiltInFunction);
        m_functionModelItems.insert(functionIndexOfFirstBuiltInFunction, functionModelItem);
        endInsertRows();
    }

    return true;
}

void FunctionModel::removeFunction(const QString &name)
{
    int functionIndex = functionModelItemIndex(name);
    if(functionIndex != -1)
    {
        beginRemoveRows(QModelIndex(), functionIndex, functionIndex);
        delete m_functionModelItems.takeAt(functionIndex);
        endRemoveRows();
    }
}

QStringList FunctionModel::functionList(FunctionModel::FunctionType type)
{
    QStringList functions;

    switch(type) {
        case Builtin:
            foreach(FunctionModelItem* functionModelItem, m_functionModelItems)
            {
                if(functionModelItem->type() == FunctionModelItem::BuiltInFunction)
                {
                    functions += functionModelItem->name();
                }
            }
            break;

        case UserDefined:
            foreach(FunctionModelItem* functionModelItem, m_functionModelItems)
            {
                if(functionModelItem->type() == FunctionModelItem::UserFunction)
                {
                    functions += functionModelItem->name();
                }
            }
            break;

        case All:
            functions += functionList(Builtin);
            functions += functionList(UserDefined);
            break;
    }

    return functions;
}

#include "functionmodel.moc"

// vim: set et sw=4 ts=8:
