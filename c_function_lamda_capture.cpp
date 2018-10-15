#include <iostream>

//
// Lambda without captures is convertible to function pointer.
// For a definition like this:
//
//  void some_c_func(void (*callback)());
//
// We can just pass lambda to above function without captures.
//
//  some_c_func([]() { ... });
//
// Using captures require lambda to be globally accessable,
// for example to be static.
//

// ------------------------------------------------------------
// Solution 1: Simple but works only for functions that
//             does not take any arguments.
// ------------------------------------------------------------
template <typename F>
auto cify_no_args(F&& f) {
    static F fn = std::forward<F>(f);
    return [] {
        return fn();
    };
}

// ------------------------------------------------------------
// Solution 2: Takes any number of arguments. Works on
//             non-mutable lambdas (or functors) only.
//             For mutable ones we need to remove const
//             in struct specialization.
// ------------------------------------------------------------
template <typename F>
struct lambda_traits : lambda_traits<decltype(&F::operator())>
{ };

template <typename F, typename R, typename... Args>
struct lambda_traits<R(F::*)(Args...)> : lambda_traits<R(F::*)(Args...) const>
{ };

template <typename F, typename R, typename... Args>
struct lambda_traits<R(F::*)(Args...) const> {
    using pointer = std::add_pointer_t<R(Args...)>;

    static pointer cify(F&& f) {
        static F fn = std::forward<F>(f);
        return [](Args... args) {
            return fn(std::forward<Args>(args)...);
        };
    }
};

template <typename F>
inline typename lambda_traits<F>::pointer cify(F&& f) {
    return lambda_traits<F>::cify(std::forward<F>(f));
}


// ------------------------------------------------------------
// Test C function for solution 1
// ------------------------------------------------------------
void some_c_func(void (*callback)()) {
    callback();
}

// ------------------------------------------------------------
// Test C function for solution 2
// ------------------------------------------------------------
struct some_struct {
    some_struct() = default;
    some_struct(const some_struct&) {
        std::cout << "copy constructor\n";
    }
    some_struct(some_struct&&) {
        std::cout << "move constructor\n";
    }
};

int some_c_func(int (*callback)(some_struct, float)) {
    return callback(some_struct(), 3.14f);
}


int main()
{
    int capture = 7;

    std::cout << "--- Solution 1:\n";

    some_c_func(cify_no_args([&capture]() {
        std::cout << "got capture: " << capture << '\n';
    }));

    std::cout << "--- Solution 2:\n";

    int ret = some_c_func(cify([&capture](some_struct s, float f) {
        std::cout << "got capture: " << capture << '\n';
        std::cout << "got s and f: " << f << '\n';
        return 42;
    }));
    std::cout << "return: " << ret << '\n';
};

