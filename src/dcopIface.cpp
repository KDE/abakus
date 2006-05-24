#include <QString>
#include <QByteArray>

#include "numerictypes.h"
#include "lexer.h"
#include "dcopIface.h"

AbakusIface::AbakusIface() : DCOPObject("Calculator")
{
}

double AbakusIface::evaluate(const QString &expr)
{
    Abakus::number_t result = parseString(expr.toUtf8().constData());
    return result.asDouble();
}
