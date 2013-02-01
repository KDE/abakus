#ifndef ABAKUS_NUMERICTYPES_H
#define ABAKUS_NUMERICTYPES_H
/*
 * numerictypes.h - part of abakus
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

#include <QString>

#include <mpfr.h>

namespace Abakus
{

/* What trigonometric mode we're in. */
typedef enum { Degrees, Radians } TrigMode;

/* What numeral system to use */
typedef enum { GLOBAL = 0, BIN = 2, OCT = 8, DEC = 10, HEX = 16} NumeralSystem;

/* Shared application-wide */
extern TrigMode m_trigMode;

/* Precision to display at. */
extern int m_prec;

/* Global numeral system */
extern NumeralSystem m_globalNumeralSystem;

/**
 * Utility function to convert a MPFR number to a string.  This is declared
 * this way so that when it changes we don't have to recompile all of Abakus.
 *
 * This function obeys the precision settings of the user.  This means that if
 * you change the precision between function calls, you may get different
 * results, even on the same number!
 *
 * But, don't use this directly, you should be using
 * Number::toString() instead!
 *
 * @param number MPFR number to convert to string.
 * @return The number converted to a string, in US Decimal format at this time.
 * @see Number::toString()
 */
QString convertToString(const mpfr_ptr& number, const Abakus::NumeralSystem base = Abakus::DEC);

/**
 * This is a specialization of the number<> template for the MPFR numeric type.
 * It uses a weird hack in that it is declared as specializing mpfr_ptr instead
 * of mpfr_t like is used everywhere in MPFR's public API.
 *
 * This is because mpfr_t does not seem to play well with C++ templates (it
 * is implemented internally as a 1-length array to get pointer semantics
 * while also allocating memory.
 *
 * What this means is that should you ever have to deal with allocating
 * memory, you need to use allocate space for it (mpfr_ptr is a pointer to
 * __mpfr_struct).
 *
 * I don't like using the internal API this way, but I have little choice.
 *
 * @author Michael Pyne <michael.pyne@kdemail.net>
 */

class Number
{
public:
    typedef mpfr_ptr value_type;

    static const mp_rnd_t RoundDirection = MPFR_RNDN;

    /// Default ctor and set-and-assign ctor wrapped in one.
    Number(const value_type& t) : m_numeralSystem(Abakus::GLOBAL)
    {
        m_t = (mpfr_ptr) new __mpfr_struct;
        mpfr_init_set(m_t, t, RoundDirection);
    }

    /// Copy constructor.
    Number(const Number &other)
    {
        m_t = (mpfr_ptr) new __mpfr_struct;
        mpfr_init_set(m_t, other.m_t, RoundDirection);
        m_numeralSystem = other.m_numeralSystem;
    }

    Number(const char *str, const int base = 10) : m_numeralSystem(Abakus::GLOBAL)
    {
        m_t = (mpfr_ptr) new __mpfr_struct;
        mpfr_init_set_str (m_t, str, base, RoundDirection);
    }

    /// Likewise
    explicit Number(const QByteArray &str, const int base = 10) : m_numeralSystem(Abakus::GLOBAL)
    {
        m_t = (mpfr_ptr) new __mpfr_struct;
        mpfr_init_set_str (m_t, str.constData(), base, RoundDirection);
    }

    /// Convienience constructor to create a Number from an integer.
    explicit Number(int i) : m_numeralSystem(Abakus::GLOBAL)
    {
        m_t = (mpfr_ptr) new __mpfr_struct;
        mpfr_init_set_si(m_t, (signed long int) i, RoundDirection);
    }

    /// Construct a Number with a value of NaN.
    Number() : m_numeralSystem(GLOBAL)
    {
        m_t = (mpfr_ptr) new __mpfr_struct;
        mpfr_init(m_t);
    }

    ~Number()
    {
        mpfr_clear(m_t);
        delete (__mpfr_struct *) m_t;
    }

    /// Assignment operator
    Number &operator=(const Number &other)
    {
        if(&other == this)
            return *this;

        mpfr_clear (m_t);
        mpfr_init_set (m_t, other.m_t, RoundDirection);

        return *this;
    }

    // Comparion operators
    bool operator!=(const Number &other) const
    {
        return mpfr_equal_p(m_t, other.m_t) == 0;
    }

    bool operator==(const Number &other) const
    {
        return mpfr_equal_p(m_t, other.m_t) != 0;
    }

    bool operator<(const Number &other) const
    {
        return mpfr_less_p(m_t, other.m_t) != 0;
    }

    bool operator>(const Number &other) const
    {
        return mpfr_greater_p(m_t, other.m_t) != 0;
    }

    bool operator<=(const Number &other) const
    {
        return mpfr_lessequal_p(m_t, other.m_t) != 0;
    }

    bool operator>=(const Number &other) const
    {
        return mpfr_greaterequal_p(m_t, other.m_t) != 0;
    }

    /// Negation operator
    Number operator -() const
    {
        Number result(m_t);
        mpfr_neg(result.m_t, result.m_t, RoundDirection);

        return result;
    }

    // internal
    Number asRadians() const
    {
        if(m_trigMode == Degrees)
        {
            Number result(m_t);
            mpfr_t pi;

            mpfr_init (pi);
            mpfr_const_pi (pi, RoundDirection);
            mpfr_mul (result.m_t, result.m_t, pi, RoundDirection);
            mpfr_div_ui (result.m_t, result.m_t, 180, RoundDirection);

            mpfr_clear (pi);

            return result;
        }
        else
            return m_t;
    }

    // internal
    Number toTrig() const
    {
        // Assumes num is in radians.
        if(m_trigMode == Degrees)
        {
            Number result(m_t);
            mpfr_t pi;

            mpfr_init (pi);
            mpfr_const_pi (pi, RoundDirection);
            mpfr_mul_ui (result.m_t, result.m_t, 180, RoundDirection);
            mpfr_div (result.m_t, result.m_t, pi, RoundDirection);

            mpfr_clear (pi);

            return result;
        }
        else
            return m_t;
    }
    
    Abakus::NumeralSystem numeralSystem()
    {
        return m_numeralSystem;
    }
    
    void setNumeralSystem(Abakus::NumeralSystem numeralSystem)
    {
        m_numeralSystem = numeralSystem;
    }

/* There is a lot of boilerplate ahead, so define a macro to declare and
 * define some functions for us to forward the call to MPFR.
 */
#define DECLARE_IMPL_BASE(name, func, in, out) Number name() const \
{ \
    Number result = in; \
    mpfr_##func (result.m_t, result.m_t, RoundDirection); \
    \
    return out; \
}

// Normal function, uses 2 rather than 3 params
#define DECLARE_NAMED_IMPL2(name, func) Number name() const \
{ \
    Number result = m_t; \
    mpfr_##func (result.m_t, result.m_t); \
    \
    return result; \
}

// Normal function, but MPFL uses a different name than abakus.
#define DECLARE_NAMED_IMPL(name, func) DECLARE_IMPL_BASE(name, func, m_t, result)

// Normal function, just routes call to MPFR.
#define DECLARE_IMPL(name) DECLARE_NAMED_IMPL(name, name)

// Trig function, degrees in
#define DECLARE_TRIG_IN_IMPL(name) DECLARE_IMPL_BASE(name, name, asRadians(), result)

// Trig function, degrees out
#define DECLARE_TRIG_OUT_IMPL(name) DECLARE_IMPL_BASE(name, name, m_t, result.toTrig())

// Now declare our functions.
    // Trigonometric, must accept values in degrees.
    DECLARE_TRIG_IN_IMPL(sin)
    DECLARE_TRIG_IN_IMPL(cos)
    DECLARE_TRIG_IN_IMPL(tan)

    // Inverse trigonometric, must return result in Degrees if necessary.
    DECLARE_IMPL(sinh)
    DECLARE_IMPL(cosh)
    DECLARE_IMPL(tanh)

    // Hyperbolic trigonometric (doesn't use Degrees).
    DECLARE_TRIG_OUT_IMPL(asin)
    DECLARE_TRIG_OUT_IMPL(acos)
    DECLARE_TRIG_OUT_IMPL(atan)

    // Inverse hyperbolic trigonometric (doesn't use degrees).
    DECLARE_IMPL(asinh)
    DECLARE_IMPL(acosh)
    DECLARE_IMPL(atanh)

    /// @return Number rounded to closest integer less than or equal to value.
    DECLARE_NAMED_IMPL2(floor, floor)
    /// @return Number rounded to closest integer greater than or equal to value.
    DECLARE_NAMED_IMPL2(ceil, ceil)
    /// @return Number with only integer component of result.
    DECLARE_NAMED_IMPL2(trunc, trunc)
    /// @return Number with only fractional component of result.
    DECLARE_IMPL(frac)
    /**
     * @return Number rounded to nearest integer.  What to do in 'strange'
     * situations is specialization-dependant, I don't really care enough to
     * mandate one or the other.
     */
    DECLARE_NAMED_IMPL2(round, round)

    /// @return Absolute value of Number.
    DECLARE_IMPL(abs)
    /// @return Square root of Number.
    DECLARE_IMPL(sqrt)
    /// @return Natural-base logarithm of value.
    DECLARE_NAMED_IMPL(ln, log)
    /// @return base-10 logarithm of value.
    DECLARE_NAMED_IMPL(log, log10)
    /// @return Natural base raised to the power given by our value.
    DECLARE_IMPL(exp)

    /// @return Our value raised to the \p exponent power.  Would be nice if
    /// it supported even exponents on negative numbers correctly.
    // Can't use macro for this one, it's sorta weird.
    Number pow(const Number &exponent)
    {
        Number result = m_t;

        mpfr_pow(result.m_t, result.m_t, exponent.m_t, RoundDirection);
        return result;
    }

    /// @return value rounded to double precision.
    double asDouble() const
    {
        return mpfr_get_d(m_t, RoundDirection);
    }
    
    /// @return number with binary representation
    Number bin() const
    {
        Number result = m_t;
        result.m_numeralSystem = Abakus::BIN;
        return result;
    }
    
    /// @return number with octal representation
    Number oct() const
    {
        Number result = m_t;
        result.m_numeralSystem = Abakus::OCT;
        return result;
    }
    
    /// @return number with decimal representation
    Number dec() const
    {
        Number result = m_t;
        result.m_numeralSystem = Abakus::DEC;
        return result;
    }
    
    /// @return number with hexadecimal representation
    Number hex() const
    {
        Number result = m_t;
        result.m_numeralSystem = Abakus::HEX;
        return result;
    }

    /// Note that this can be used dangerously, be careful.
    /// @return Our value.
    value_type value() const { return m_t; }

    /// @return Textual representation of the Number, adjusted to the user's
    /// current precision.
    QString toString() const
    {
        QString number;
        // Move this to .cpp to avoid recompiling as I fix it.
        switch(m_numeralSystem)
        {
            case GLOBAL:
                number = convertToString(m_t, m_globalNumeralSystem);
                break;
            default:
                number = convertToString(m_t, m_numeralSystem);
        }
        return number;
    }
    
    QString toString(Abakus::NumeralSystem numeralSystem) const
    {
        return convertToString(m_t, numeralSystem);
    }

    static Number nan()
    {
        // Doesn't apply, but the default value when initialized happens
        // to be nan.
        return Number();
    }

    static const value_type PI;
    static const value_type E;

private:
    mpfr_ptr m_t;
    NumeralSystem m_numeralSystem;
};

// Specializations of math operators for mpfr.

inline Number operator+(const Number &l, const Number &r)
{
    Number result;
    mpfr_add(result.value(), l.value(), r.value(), GMP_RNDN);

    return result;
}

inline Number operator-(const Number &l, const Number &r)
{
    Number result;
    mpfr_sub(result.value(), l.value(), r.value(), GMP_RNDN);

    return result;
}

inline Number operator*(const Number &l, const Number &r)
{
    Number result;
    mpfr_mul(result.value(), l.value(), r.value(), GMP_RNDN);

    return result;
}

inline Number operator/(const Number &l, const Number &r)
{
    Number result;
    mpfr_div(result.value(), l.value(), r.value(), GMP_RNDN);

    return result;
}

}; // namespace Abakus

#endif /* ABAKUS_NUMERICTYPES_H */

// vim: set et ts=8 sw=4:
