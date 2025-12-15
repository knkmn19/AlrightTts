// { expected.hpp } : expecte helper (its just the one from tk
// Unlike tk though theres no like rt so using unlikely over unreachable
// 

#ifndef ALRTTS_EXPECTED_HPP
#define ALRTTS_EXPECTED_HPP

#if (_MSC_VER > 1000)
#   pragma once
#
#endif // (_MSC_VER > 1000)

#include "annotations.h"

template<typename T, typename U>
class Expected final {
private:
    bool m_bExpected;
    union {
        T m_value;
        U m_error;
    };

public:
    Expected(T) noexcept;
    Expected(U) noexcept;

    operator bool(void) const noexcept;

    T& operator*(void) &noexcept;
    T const& operator*(void) const& noexcept;

    U Error(void) const noexcept;
};

template<typename T, typename U>
Expected<T, U>::Expected(T v) noexcept
{
    m_bExpected = true;
    m_value = v;
}

template<typename T, typename U>
Expected<T, U>::Expected(U e) noexcept
{
    m_bExpected = false;
    m_error = e;
}

template<typename T, typename U>
Expected<T, U>::operator bool(void) const noexcept
    { return m_bExpected; }

template<typename T, typename U>
T& Expected<T, U>::operator*(void) &noexcept
{
    if CMP_UNLIKELY (!m_bExpected)
        ;//::tk_unreachable();
    return m_value;
}

template<typename T, typename U>
T const& Expected<T, U>::operator*(void) const& noexcept
    { return *(*this); }

template<typename T, typename U>
U Expected<T, U>::Error(void) const noexcept
{
    if CMP_UNLIKELY (m_bExpected)
        ;//::tk_unreachable();
    return m_error;
}

#endif // !ALRTTS_EXPECTED_HPP

