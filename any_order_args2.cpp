#include <tuple>
#include <iostream>
#include <string>
#include "args_of.hpp"


struct none_type { };


template <class L, class T>
struct find_decay;

template <class L, class T>
using find_decay_t = typename find_decay<L, T>::type;

template <template <class, class...> class L, class Head, class... Tail, class T>
struct find_decay<L<Head, Tail...>, T>
: std::conditional<
    std::is_same_v<std::decay_t<Head>, std::decay_t<T>>,
    Head,
    find_decay_t<L<Tail...>, T> >
{ };

template <template <class...> class L, class T>
struct find_decay<L<>, T> {
    using type = none_type;
};


template <class T>
struct is_none : std::is_same<T, none_type> { };

template <class T>
constexpr bool is_none_v = is_none<T>::value;


template <class... T>
struct opt_invoker
{
    // If FnArg not in ArgsTuple (value not provided) return decay type of FnArg
    template <class FnArg, class ArgsTuple, class ArgType = find_decay_t<ArgsTuple, FnArg>>
    std::enable_if_t<is_none_v<ArgType>, std::decay_t<FnArg>>
    get_or(ArgsTuple&) {
        return { };
    }

    // If FnArg in ArgsTuple (value is provided) return Type and Value of ArgsTuple
    template <class FnArg, class ArgsTuple, class ArgType = find_decay_t<ArgsTuple, FnArg>>
    std::enable_if_t<not is_none_v<ArgType>, ArgType>
    get_or(ArgsTuple& args) {
        return std::get<ArgType>(args);
    }

    template <class F, class... Args>
    decltype(auto) operator()(F&& f, Args&&...args) {
        std::tuple<Args...> targs(std::forward<Args>(args)...);
        return f(get_or<T>(targs)...);
    }
};


template <class F, class... Args>
inline decltype(auto) opt_invoke(F&& f, Args&&...args) {
    args_of_t<F, opt_invoker> invoker;
    return invoker(std::forward<F>(f), std::forward<Args>(args)...);
}





struct other_struct {
    int x = 42;
};

void processing_fn(int& i, const std::string& s, other_struct os)
{
    std::cout << "i [" << i << "], s [" << s << "], os [" << os.x << "]\n";
    i += 2;
}

template <class... Args>
void test_args(Args&&... args)
{
    using targs = std::tuple<Args...>;
    static_assert(std::is_same_v<targs, std::tuple<const int&, std::string, other_struct>>);
}


int main()
{
    using test = std::tuple<const int&, float>;

    static_assert(std::is_same_v< find_decay_t< test, int&& >, const int& >);
    static_assert(std::is_same_v< find_decay_t< test, char >, none_type >);

    static_assert(is_none_v< find_decay_t<test, char> >);

    int ii = 5;
    opt_invoke(processing_fn, std::string("test"), ii);
    std::cout << "ii after: " << ii << '\n';
}

