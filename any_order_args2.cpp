#include <tuple>
#include <functional>
#include <iostream>
#include <string>
#include "args_of.hpp"


template <class... T>
struct opt_invoke_impl {
private:
    // A type when no match found
    struct none_type { };

    template <class U>
    using is_none = std::is_same<U, none_type>;

    // Find U in L using decayed match
    template <class L, class U>
    struct find_decay;

    template <template <class, class...> class L, class Head, class... Tail, class U>
    struct find_decay<L<Head, Tail...>, U>
    : std::conditional<
        std::is_same_v<std::decay_t<Head>, std::decay_t<U>>,
        Head,
        typename find_decay<L<Tail...>, U>::type >
    { };
      
    template <template <class...> class L, class U>
    struct find_decay<L<>, U> {
        using type = none_type;
    };

    // Return argument provided in invoke() if present,
    // otherwise create and return default one.
    template <class FnArg, class ArgsTuple>
    static decltype(auto) get_value(ArgsTuple& args) {
        using ArgType = typename find_decay<ArgsTuple, FnArg>::type;
        if constexpr (is_none<ArgType>::value)
            // Required arg not provided, create a default one
            return std::decay_t<FnArg> { };
        else
            return std::forward<ArgType>(std::get<ArgType>(args));
    }

public:
    // Member function pointer
    template <class F, class C, class Arg, class... Args>
    static decltype(auto) invoke(F C::* f, Arg&& arg, Args&&...args) {
        std::tuple<Args...> targs(std::forward<Args>(args)...);
        return std::invoke(f, std::forward<Arg>(arg), get_value<T>(targs)...);
    }

    // Non-member function
    template <class F, class... Args>
    static decltype(auto) invoke(F&& f, Args&&...args) {
        std::tuple<Args...> targs(std::forward<Args>(args)...);
        return f(get_value<T>(targs)...);
    }
};


template <class F, class... Args>
inline decltype(auto) opt_invoke(F&& f, Args&&...args) {
    // TODO check F contains unique arg types
    args_of_t<F, opt_invoke_impl>::invoke(
        std::forward<F>(f), std::forward<Args>(args)...);
}



// Tests

struct other_struct {
    void method(std::string s, int i, float f) {
        std::cout << "s [" << s << "], i [" << i << "], f [" << f << "]\n";
    }
    int x = 42;
};

void processing_fn(int& i, const std::string& s, other_struct os)
{
    std::cout << "i [" << i << "], s [" << s << "], os [" << os.x << "]\n";
    i += 2;
}

/* Move this above opt_invoke_impl and change "return f(...)" to "return test_args(...)"
template <class... Args>
void test_args(Args&&...)
{
    using targs = std::tuple<Args...>;
    static_assert(std::is_same_v<targs, std::tuple<int&, std::string, other_struct>>);
}
*/


int main()
{
/*
    using test = std::tuple<const int&, float>;

    static_assert(std::is_same_v< find_decay_t< test, int&& >, const int& >);
    static_assert(std::is_same_v< find_decay_t< test, char >, none_type >);

    static_assert(is_none_v< find_decay_t<test, char> >);
*/

    int ii = 5;
    opt_invoke(processing_fn, std::string("test"), ii);
    std::cout << "ii after: " << ii << '\n';

    other_struct os;
    std::string s("method");
    opt_invoke(&other_struct::method, &os, 10, s);
}

