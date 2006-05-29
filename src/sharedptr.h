#ifndef ABAKUS_SHARED_PTR_H
#define ABAKUS_SHARED_PTR_H
/*
 * sharedptr.h - part of abakus
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

template <class T>
class SharedPtr
{
public:
    typedef T value_type;

    class Data
    {
    public:
        Data(T *ptr = 0, unsigned refCount = 1) : m_ptr(ptr), m_refCount(refCount)
        {
        }

        void deref()
        {
            --m_refCount;

            if(!m_refCount) {
                delete m_ptr;
                m_ptr = 0;
            }
        }

        void ref()
        {
            ++m_refCount;
        }

        T *ptr() { return m_ptr; }
        const T* ptr() const { return m_ptr; }

        unsigned refCount() const { return m_refCount; }

    private:
        T *m_ptr;
        unsigned m_refCount;
    };

    SharedPtr() : m_data(new Data)
    {
    }

    SharedPtr(T* ptr) : m_data(new Data(ptr))
    {
    }

    SharedPtr(const SharedPtr<T> &other) : m_data(other.m_data)
    {
        m_data->ref();
    }

    ~SharedPtr()
    {
        m_data->deref();
    }

    SharedPtr<T> &operator=(const SharedPtr<T> &other)
    {
        if(&other == this)
            return *this;

        m_data->deref();
        m_data = other.m_data;
        m_data->ref();

        return *this;
    }

    T *operator ->()
    {
        return m_data->ptr();
    }

    const T *operator ->() const
    {
        return m_data->ptr();
    }

    T &operator *()
    {
        return *m_data->ptr();
    }

    const T& operator *() const
    {
        return *m_data->ptr();
    }

    bool isNull() const { return m_data->ptr() == 0; }

    unsigned refCount() const { return m_data->refCount(); }

private:
    Data *m_data;
};

#endif /* ABAKUS_SHARED_PTR_H */

// vim: set et ts=8 sw=4:
