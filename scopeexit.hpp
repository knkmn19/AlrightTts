// { scopeexit.hpp } : scope exit helper (exactly thej same as the tk version
// 

#ifndef ALRTTS_SCOPEEXIT_HPP
#define ALRTTS_SCOPEEXIT_HPP

#if (_MSC_VER > 1000)
#   pragma once
#
#endif // (_MSC_VER > 1000)

template<typename TLambda>
class ScopeExit final {
private:
    TLambda m_fn;
    bool m_bActive;

public:
    ScopeExit(TLambda) noexcept;
    ~ScopeExit(void) noexcept;

    void Run(void) noexcept;
    void Cancel(void) noexcept;
};

template<typename L>
::ScopeExit<L>::ScopeExit(L fn) noexcept
    : m_fn(fn),
      m_bActive(true)
    { ; }

template<typename L>
::ScopeExit<L>::~ScopeExit(void) noexcept
{
    if (!m_bActive)
        return;
    m_fn();
}

template<typename L>
void ::ScopeExit<L>::Run(void) noexcept
    { m_fn(); }

template<typename L>
void ::ScopeExit<L>::Cancel(void) noexcept
    { m_bActive = false; }

#endif // !ALRTTS_SCOPEEXIT_HPP
