#include <iostream>

struct request { };
struct response { };

// All args should have unique type signature
// in order to distinguish them.

template <typename... Args>
void test(Args&&... args)
{
    auto process_arg = [](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;

        if constexpr (std::is_same_v<T, request>) {
            std::cout << "has request\n";
        }
        else if constexpr (std::is_same_v<T, response>) {
            std::cout << "has response\n";
        }
        else {
            std::cout << "unknown type\n";
        }
    };

    (process_arg(std::forward<Args>(args)), ...);
}


int main()
{
    std::cout << "--- test 1:\n";
    test(response(), 5, request());

    std::cout << "--- test 2:\n";
    test(request(), response());
}

