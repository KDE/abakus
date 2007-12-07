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

#include <sstream>
#include <string>

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QRegExp>

#include "hmath.h"
#include "config-abakus.h"

#ifdef HAVE_MPFR
#include <mpfr.h>
#endif

namespace Abakus
{

/* What trigonometric mode we're in. */
typedef enum { Degrees, Radians } TrigMode;

/* Shared application-wide */
extern TrigMode m_trigMode;

/* Precision to display at. */
extern int m_prec;

/**
 * Representation of a number type.  Includes the basic operators, along with
 * built-in functions such as abs() and mod().
 *
 * You need to actually define it using template specializations though.  You
 * can add functions in a specialization, it may be worth it to have the
 * functions declared here as well so that you get a compiler error if you
 * forget to implement it.
 *
 * Note that since we're using a specialization, and then typedef'ing the
 * new specialized class to number_t, that means we only support one type of
 * number at a time, and the choice is made at compile-time.
 */
template <typename T>
class number
{
public:
    /// Default ctor and set-and-assign ctor wrapped in one.
    number(const T& t = T());

    /// Copy constructor.
    number(const number &other);

    /// Create number from textual representation, useful for ginormously
    /// precise numbers.
    number(const char *str);

    /// Likewise
    explicit number(const QByteArray &str);

    /// Convienience constructor to create a number from an integer.
    explicit number(int i);

    /// Assignment operator.  Be sure to check for &other == this if necessary!
    number<T> &operator =(const number<T> &other);

    // You need to implement the suite of comparison operators as well, along
    // with the negation operator.  Sorry.

    bool operator!=(const number<T> &other) const;
    bool operator==(const number<T> &other) const;
    bool operator<(const number<T> &other) const;
    bool operator>(const number<T> &other) const;
    bool operator<=(const number<T> &other) const;
    bool operator>=(const number<T> &other) const;

    number<T> operator -() const;

    // These functions must be implemented by all specializations to be used.
    // Note that when implementing these functions, the implicit value is the
    // value that this object is wrapping.  E.g. you'd call the function on
    // a number object, kind of like 3.sin() if you were using Ruby.

    // Trigonometric, must accept values in degrees.
    number<T> sin() const;
    number<T> cos() const;
    number<T> tan() const;

    // Inverse trigonometric, must return result in Degrees if necessary.
    number<T> asin() const;
    number<T> acos() const;
    number<T> atan() const;

    // Hyperbolic trigonometric (doesn't use Degrees).
    number<T> sinh() const;
    number<T> cosh() const;
    number<T> tanh() const;

    // Inverse hyperbolic trigonometric (doesn't use degrees).
    number<T> asinh() const;
    number<T> acosh() const;
    number<T> atanh() const;

    /// @return Number rounded to closest integer less than or equal to value.
    number<T> floor() const;

    /// @return Number rounded to closest integer greater than or equal to value.
    number<T> ceil() const;

    /// @return Number with only integer component of result.
    number<T> integer() const;

    /// @return Number with only fractional component of result.
    number<T> frac() const;

    /**
     * @return Number rounded to nearest integer.  What to do in 'strange'
     * situations is specialization-dependant, I don't really care enough to
     * mandate one or the other.
     */
    number<T> round() const;

    /// @return Absolute value of number.
    number<T> abs() const;

    /// @return Square root of number.
    number<T> sqrt() const;

    /// @return Natural-base logarithm of value.
    number<T> ln() const;

    /// @return base-10 logarithm of value.
    number<T> log() const;

    /// @return Natural base raised to the power given by our value.
    number<T> exp() const;

    /// @return Our value raised to the \p exponent power.  Would be nice if
    /// it supported even exponents on negative numbers correctly.
    number<T> pow(const number<T> &exponent);

    /// @return value rounded to double precision.
    double asDouble() const;

    /// @return Textual representation of the number, adjusted to the user's
    /// current precision.
    QString toString() const;

    /// @return Our value.
    T value() const;
};

// You should also remember to overload the math operators for your
// specialization.  These generic ones should work for templates wrapping a
// type that C++ already has operators for.

template<typename T>
inline number<T> operator+(const number<T> &l, const number<T> &r)
{
    return number<T>(l.value() + r.value());
}

template<typename T>
inline number<T> operator-(const number<T> &l, const number<T> &r)
{
    return number<T>(l.value() - r.value());
}

template<typename T>
inline number<T> operator*(const number<T> &l, const number<T> &r)
{
    return number<T>(l.value() * r.value());
}

template<typename T>
inline number<T> operator/(const number<T> &l, const number<T> &r)
{
    return number<T>(l.value() / r.value());
}

#ifdef HAVE_MPFR

/**
 * Utility function to convert a MPFR number to a string.  This is declared
 * this way so that when it changes we don't have to recompile all of Abakus.
 *
 * This function obeys the precision settings of the user.  This means that if
 * you change the precision between function calls, you may get different
 * results, even on the same number!
 *
 * But, don't use this directly, you should be using
 * number<mpfr_ptr>::toString() instead!
 *
 * @param number MPFR number to convert to string.
 * @return The number converted to a string, in US Decimal format at this time.
 * @see number<>::toString()
 */
QString convertToString(const mpfr_ptr &number);

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
template<>
class number<mpfr_ptr>
{
public:
    typedef mpfr_ptr value_type;

    static const mp_rnd_t RoundDirection = GMP_RNDN;

    number(const value_type& t)
    {
        m_t = (mpfr_ptr) new __mpfr_struct;
        mpfr_init_set(m_t, t, RoundDirection);
    }

    number(const number<value_type> &other)
    {
        m_t = (mpfr_ptr) new __mpfr_struct;
        mpfr_init_set(m_t, other.m_t, RoundDirection);
    }

    number(const char *str)
    {
        m_t = (mpfr_ptr) new __mpfr_struct;
        mpfr_init_set_str (m_t, str, 10, RoundDirection);
    }

    explicit number(const QByteArray &str)
    {
        m_t = (mpfr_ptr) new __mpfr_struct;
        mpfr_init_set_str (m_tr, str.constData(), 10, RoundDirection);
    }

    explicit number(int i)
    {
        m_t = (mpfr_ptr) new __mpfr_struct;
        mpfr_init_set_si(m_t, (signed long int) i, RoundDirection);
    }

    /// Construct a number with a value of NaN.
    number()
    {
        m_t = (mpfr_ptr) new __mpfr_struct;
        mpfr_init(m_t);
    }

    ~number()
    {
        mpfr_clear(m_t);
        delete (__mpfr_struct *) m_t;
    }

    number<value_type> &operator=(const number<value_type> &other)
    {
        if(&other == this)
            return *this;

        mpfr_clear (m_t);
        mpfr_init_set (m_t, other.m_t, RoundDirection);

        return *this;
    }

    bool operator!=(const number<value_type> &other) const
    {
        return mpfr_equal_p(m_t, other.m_t) == 0;
    }

    bool operator==(const number<value_type> &other) const
    {
        return mpfr_equal_p(m_t, other.m_t) != 0;
    }

    bool operator<(const number<value_type> &other) const
    {
        return mpfr_less_p(m_t, other.m_t) != 0;
    }

    bool operator>(const number<value_type> &other) const
    {
        return mpfr_greater_p(m_t, other.m_t) != 0;
    }

    bool operator<=(const number<value_type> &other) const
    {
        return mpfr_lessequal_p(m_t, other.m_t) != 0;
    }

    bool operator>=(const number<value_type> &other) const
    {
        return mpfr_greaterequal_p(m_t, other.m_t) != 0;
    }

    number<value_type> operator -() const
    {
        number<value_type> result(m_t);
        mpfr_neg(result.m_t, result.m_t, RoundDirection);

        return result;
    }

    // internal
    number<value_type> asRadians() const
    {
        if(m_trigMode == Degrees)
        {
            number<value_type> result(m_t);
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
    number<value_type> toTrig() const
    {
        // Assumes num is in radians.
        if(m_trigMode == Degrees)
        {
            number<value_type> result(m_t);
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

/* There is a lot of boilerplate ahead, so define a macro to declare and
 * define some functions for us to forward the call to MPFR.
 */
#define DECLARE_IMPL_BASE(name, func, in, out) number<value_type> name() const \
{ \
    number<value_type> result = in; \
    mpfr_##func (result.m_t, result.m_t, RoundDirection); \
    \
    return out; \
}

// Normal function, uses 2 rather than 3 params
#define DECLARE_NAMED_IMPL2(name, func) number<value_type> name() const \
{ \
    number<value_type> result = m_t; \
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
    DECLARE_TRIG_IN_IMPL(sin)
    DECLARE_TRIG_IN_IMPL(cos)
    DECLARE_TRIG_IN_IMPL(tan)

    DECLARE_IMPL(sinh)
    DECLARE_IMPL(cosh)
    DECLARE_IMPL(tanh)

    DECLARE_TRIG_OUT_IMPL(asin)
    DECLARE_TRIG_OUT_IMPL(acos)
    DECLARE_TRIG_OUT_IMPL(atan)

    DECLARE_IMPL(asinh)
    DECLARE_IMPL(acosh)
    DECLARE_IMPL(atanh)

    DECLARE_NAMED_IMPL2(floor, floor)
    DECLARE_NAMED_IMPL2(ceil, ceil)
    DECLARE_NAMED_IMPL(integer, rint)
    DECLARE_IMPL(frac)
    DECLARE_NAMED_IMPL2(round, round)

    DECLARE_IMPL(abs)
    DECLARE_IMPL(sqrt)
    DECLARE_NAMED_IMPL(ln, log)
    DECLARE_NAMED_IMPL(log, log10)
    DECLARE_IMPL(exp)

    // Can't use macro for this one, it's sorta weird.
    number<value_type> pow(const number<value_type> &exponent)
    {
        number<value_type> result = m_t;

        mpfr_pow(result.m_t, result.m_t, exponent.m_t, RoundDirection);
        return result;
    }

    double asDouble() const
    {
        return mpfr_get_d(m_t, RoundDirection);
    }

    // Note that this can be used dangerously, be careful.
    value_type value() const { return m_t; }

    QString toString() const
    {
        // Move this to .cpp to avoid recompiling as I fix it.
        return convertToString(m_t);
    }

    static number<value_type> nan()
    {
        // Doesn't apply, but the default value when initialized happens
        // to be nan.
        return number<value_type>();
    }

    static const value_type PI;
    static const value_type E;

private:
    mpfr_ptr m_t;
};

// Specializations of math operators for mpfr.

template<>
inline number<mpfr_ptr> operator+(const number<mpfr_ptr> &l, const number<mpfr_ptr> &r)
{
    number<mpfr_ptr> result;
    mpfr_add(result.value(), l.value(), r.value(), GMP_RNDN);

    return result;
}

template<>
inline number<mpfr_ptr> operator-(const number<mpfr_ptr> &l, const number<mpfr_ptr> &r)
{
    number<mpfr_ptr> result;
    mpfr_sub(result.value(), l.value(), r.value(), GMP_RNDN);

    return result;
}

template<>
inline number<mpfr_ptr> operator*(const number<mpfr_ptr> &l, const number<mpfr_ptr> &r)
{
    number<mpfr_ptr> result;
    mpfr_mul(result.value(), l.value(), r.value(), GMP_RNDN);

    return result;
}

template<>
inline number<mpfr_ptr> operator/(const number<mpfr_ptr> &l, const number<mpfr_ptr> &r)
{
    number<mpfr_ptr> result;
    mpfr_div(result.value(), l.value(), r.value(), GMP_RNDN);

    return result;
}

    // Abakus namespace continues.
    typedef number<mpfr_ptr> number_t;

#else

// Defined in numerictypes.cpp for ease of reimplementation.
QString convertToString(const HNumber &num);

/**
 * Specialization for internal HMath library, used if MPFR isn't usable.
 *
 * @author Michael Pyne <michael.pyne@kdemail.net>
 */
template<>
class number<HNumber>
{
public:
    typedef HNumber value_type;

    number(const HNumber& t = HNumber()) : m_t(t)
    {
    }
    explicit number(int i) : m_t(i) { }
    number(const number<HNumber> &other) : m_t(other.m_t) { }

    /// Likewise
    explicit number(const QByteArray &str) : m_t(str.constData()) { }

    number(const char *s) : m_t(s) { }

    bool operator!=(const number<HNumber> &other) const
    {
        return m_t != other.m_t;
    }

    bool operator==(const number<HNumber> &other) const
    {
        return m_t == other.m_t;
    }

    bool operator<(const number<HNumber> &other) const
    {
        return m_t < other.m_t;
    }

    bool operator>(const number<HNumber> &other) const
    {
        return m_t > other.m_t;
    }

    bool operator<=(const number<HNumber> &other) const
    {
        return m_t <= other.m_t;
    }

    bool operator>=(const number<HNumber> &other) const
    {
        return m_t >= other.m_t;
    }

    number<HNumber> &operator=(const number<HNumber> &other)
    {
        m_t = other.m_t;
        return *this;
    }

    HNumber asRadians() const
    {
        if(m_trigMode == Degrees)
            return m_t * PI / HNumber("180.0");
        else
            return m_t;
    }

    HNumber toTrig(const HNumber &num) const
    {
        // Assumes num is in radians.
        if(m_trigMode == Degrees)
            return num * HNumber("180.0") / PI;
        else
            return num;
    }

    number<HNumber> sin() const
    {
        return HMath::sin(asRadians());
    }

    number<HNumber> cos() const
    {
        return HMath::cos(asRadians());
    }

    number<HNumber> tan() const
    {
        return HMath::tan(asRadians());
    }

    number<HNumber> asin() const
    {
        return toTrig(HMath::asin(m_t));
    }

    number<HNumber> acos() const
    {
        return toTrig(HMath::acos(m_t));
    }

    number<HNumber> atan() const
    {
        return toTrig(HMath::atan(m_t));
    }

    number<HNumber> floor() const
    {
        if(HMath::frac(m_t) == HNumber("0.0"))
            return integer();
        if(HMath::integer(m_t) < HNumber("0.0"))
            return HMath::integer(m_t) - 1;
        return integer();
    }

    number<HNumber> ceil() const
    {
        return floor().value() + HNumber(1);
    }

/* There is a lot of boilerplate ahead, so define a macro to declare and
 * define some functions for us to forward the call to HMath.
 */
#define DECLARE_IMPL(name) number<value_type> name() const \
{ return HMath::name(m_t); }

    DECLARE_IMPL(frac)
    DECLARE_IMPL(integer)
    DECLARE_IMPL(round)

    DECLARE_IMPL(abs)

    DECLARE_IMPL(sqrt)

    DECLARE_IMPL(ln)
    DECLARE_IMPL(log)
    DECLARE_IMPL(exp)

    DECLARE_IMPL(sinh)
    DECLARE_IMPL(cosh)
    DECLARE_IMPL(tanh)

    DECLARE_IMPL(asinh)
    DECLARE_IMPL(acosh)
    DECLARE_IMPL(atanh)

    HNumber value() const { return m_t; }

    double asDouble() const { return toString().toDouble(); }

    number<HNumber> operator-() const { return HMath::negate(m_t); }

    // TODO: I believe this doesn't work for negative numbers with even
    // exponents.  Which breaks simple stuff like (-2)^2. :(
    number<HNumber> pow(const number<HNumber> &exponent)
    {
        return HMath::raise(m_t, exponent.m_t);
    }

    QString toString() const
    {
        return convertToString(m_t);
    }

    static number<HNumber> nan()
    {
        return HNumber::nan();
    }

    static const HNumber PI;
    static const HNumber E;

private:
    HNumber m_t;
};

    // Abakus namespace continues.
    typedef number<HNumber> number_t;

#endif /* HAVE_MPFR */

}; // namespace Abakus

#endif /* ABAKUS_NUMERICTYPES_H */

// vim: set et ts=8 sw=4:
