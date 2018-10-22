// Extract function types

#pragma once
#include <type_traits>

// Primary template
template <class F>
struct fn_traits : fn_traits<typename std::remove_pointer<F>::type> { };

// Specialization for member functions
template<class T, class F>
struct fn_traits<F T::*> : fn_traits<F> { };

// Specialization for function types that have cv-qualifiers
template <class R, class... A>
struct fn_traits<R(A...) const> : fn_traits<R(A...)> { };
template <class R, class... A>
struct fn_traits<R(A...) volatile> : fn_traits<R(A...)> { };
template <class R, class... A>
struct fn_traits<R(A...) const volatile> : fn_traits<R(A...)> { };

// Specializations for noexcept versions of all the above (C++17 and later)
#if (__cplusplus >= 201703L)
template <class R, class... A>
struct fn_traits<R(A...) noexcept> : fn_traits<R(A...)> { };
template <class R, class... A>
struct fn_traits<R(A...) const noexcept> : fn_traits<R(A...)> { };
template <class R, class... A>
struct fn_traits<R(A...) volatile noexcept> : fn_traits<R(A...)> { };
template <class R, class... A>
struct fn_traits<R(A...) const volatile noexcept> : fn_traits<R(A...)> { };
#endif

template <class R, class... A>
struct fn_traits<R(A...)> {
    using reduced_type = R(A...);
    using return_type = R;

    // Store arguments in a type list, ex. std::tuple
    template <template <class...> class C>
    using arguments = C<A...>;
};

