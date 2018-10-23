// Extract function argument types

#pragma once
#include <type_traits>

// Primary template
template <class F, template <class...> class C>
struct args_of : args_of<typename std::remove_pointer<F>::type, C> { };

// Specialization for member functions
template<class T, class F, template <class...> class C>
struct args_of<F T::*, C> : args_of<F, C> { };

// Specialization for function types that have cv-qualifiers

template <class R, class... A, template <class...> class C>
struct args_of<R(A...) const, C> : args_of<R(A...), C> { };

template <class R, class... A, template <class...> class C>
struct args_of<R(A...) volatile, C> : args_of<R(A...), C> { };

template <class R, class... A, template <class...> class C>
struct args_of<R(A...) const volatile, C> : args_of<R(A...), C> { };

// Specializations for noexcept versions of all the above (C++17 and later)
#if (__cplusplus >= 201703L)

template <class R, class... A, template <class...> class C>
struct args_of<R(A...) noexcept, C> : args_of<R(A...), C> { };

template <class R, class... A, template <class...> class C>
struct args_of<R(A...) const noexcept, C> : args_of<R(A...), C> { };

template <class R, class... A, template <class...> class C>
struct args_of<R(A...) volatile noexcept, C> : args_of<R(A...), C> { };

template <class R, class... A, template <class...> class C>
struct args_of<R(A...) const volatile noexcept, C> : args_of<R(A...), C> { };

#endif

template <class R, class... A, template <class...> class C>
struct args_of<R(A...), C> {
    using type = C<A...>;
};

template <class F, template <class...> class C>
using args_of_t = typename args_of<F, C>::type;

