/*
 * numerictypes.cpp - part of abakus
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

#include <KLocale>
#include <KGlobal>

#include <QRegExp>

Abakus::TrigMode Abakus::m_trigMode = Abakus::Degrees;
int Abakus::m_prec = -1;

namespace Abakus
{

QString convertToString(const mpfr_ptr &number)
{
    char *str = 0;
    QRegExp zeroKiller ("0*$");
    mp_exp_t exp;
    int desiredPrecision = Abakus::m_prec;
    QString decimalSymbol = KGlobal::locale()->decimalSymbol();

    if(desiredPrecision < 0)
        desiredPrecision = 8;

    // This first call is to see approximately how many digits of precision
    // the fractional part has.
    str = mpfr_get_str (0, &exp, 10, desiredPrecision, number, GMP_RNDN);

    // Check for ginormously small numbers.
    if(exp < -74)
        return "0";

    if(exp < -2 || exp > desiredPrecision)
    {
        // Use exponential notation.
        QString numbers (str);
        mpfr_free_str(str);

        QString sign, l, r;
        if(numbers[0] == '-')
        {
            sign = "-";
            l = numbers[1];
            r = numbers.right(numbers.length() - 2);
        }
        else
        {
            l = numbers[0];
            r = numbers.right(numbers.length() - 1);
        }

        // Remove trailing zeroes.
        if(Abakus::m_prec < 0)
            r.replace(zeroKiller, "");

        // But don't display numbers like 2.e10 either.
        if(r.isEmpty())
            r = "0";

        r.append(QString("e%1").arg(exp - 1));

        return sign + l + decimalSymbol + r;
    }
    else
    {
        mpfr_free_str(str);

        // This call is to adjust the result so that the fractional part has at
        // most m_prec digits of precision.
        str = mpfr_get_str (0, &exp, 10, exp + desiredPrecision, number, GMP_RNDN);
    }

    QString result = str;
    mpfr_free_str(str);
    str = 0;

    int position = exp;
    QString l, r, sign;

    if(position < 0) { // Number < 0.1
        l.fill('0', -position);

        if(result[0] == '-') {
            sign = "-";
            r = result.right(result.length() - 1);
        }
        else
            r = result;

        r = l + r;
        l = '0';
    }
    else { // Number >= 0.1
        if(result[0] == '-') {
            l = result.mid(1, position);
            sign = "-";
            position++;
        }
        else
            l = result.left(position);

        r = result.right(result.length() - position);
    }

    // Remove trailing zeroes.
    r.replace(zeroKiller, "");

    // Don't display numbers of the form .23
    if(l.isEmpty())
        l = "0";

    // If we have an integer don't display the decimal part.
    if(r.isEmpty())
        return sign + l;

    return sign + l + decimalSymbol + r;
}

} // namespace Abakus

Abakus::Number::value_type setupPi()
{
    static mpfr_t pi;

    mpfr_init2 (pi, 250);
    mpfr_const_pi (pi, GMP_RNDN);

    return pi;
}

Abakus::Number::value_type setupExponential()
{
    static mpfr_t exponential;
    mpfr_t one;

    mpfr_init2 (exponential, 250);
    mpfr_init_set_ui (one, 1, GMP_RNDN);
    mpfr_exp (exponential, one, GMP_RNDN);
    mpfr_clear (one);

    return exponential;
}

const Abakus::Number::value_type Abakus::Number::PI = setupPi();
const Abakus::Number::value_type Abakus::Number::E = setupExponential();

// vim: set et ts=8 sw=4:
