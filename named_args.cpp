#include <iostream>
#include <string_view>
#include <tuple>


// Global instantiation is the key to named arguments.
// At least to get exactly the Python style.
// To get rid of globals (and be thread safe) we could
// use parentheses, like
//
//   fn(name() = "test");
//
// where "name" is a type (or an alias to type), ex:
//
//   using name = std::string_view;
//
std::string_view name;
int value = 0;


// Don't care about passed argument
template <typename... Args>
void dont_care(const Args&...)
{
    std::cout << name << " number is " << value << '\n';
}


// Naive way of argument extract.
template <typename... Args>
void naive_do_care(const Args&... args)
{
    std::string_view name_arg;
    int value_arg = 0;

    auto assign_arg = [&](const auto& arg) {
        using T = std::decay_t<decltype(arg)>;

        if constexpr (std::is_same_v<T, std::string_view>)
            name_arg = arg;
        else if constexpr (std::is_same_v<T, int>)
            value_arg = arg;
    };

    (assign_arg(args), ...);
    std::cout << name_arg << " number is " << value_arg << '\n';
}


// Ok! Require that all used arguments specified.
// No check for other types except duplicates.
template <typename... Args>
void tuple_do_care(Args&&... args)
{
    // Copy/Move passed arguments. To avoid localy store
    // arguments use std::forward_as_tuple and decay
    // std::get. See tuple2_do_care().
    auto tuple_args = std::make_tuple(std::forward<Args>(args)...);

    std::cout << std::get<std::string_view>(tuple_args)
              << " number is " << std::get<int>(tuple_args) << '\n';
}


// Better one! Has default values, don't require all arguments and
// reject types not listed in "needed_args".
template <typename... Args>
void tuple2_do_care(Args&&... args)
{
    // Accepted args with default values
    std::tuple<std::string_view, int> needed_args;

    // Argument references passed to this functions
    auto tuple_args = std::forward_as_tuple(std::forward<Args>(args)...);

    // Copy/Move passed arguments
    auto set = [&needed_args](auto&& arg) {
        using T = decltype(arg);
        std::get<std::decay_t<T>>(needed_args) = std::forward<T>(arg);
    };
    (set(std::get<Args>(tuple_args)), ...);

    // Print
    std::cout << std::get<std::string_view>(needed_args)
              << " number is " << std::get<int>(needed_args) << '\n';
}


int main()
{
    // First solution don't care about passed argument,
    // it use the globals instead.

    std::cout << "--- Solution 1:\n";

    dont_care(name = "universal", value = 42);
    // Prints the same in the same order as above
    dont_care(value = 42, name = "universal");

    // Other solutions based on the fact that each argument
    // after assignment pass it's value and it's type.
    // This is how function distinguish argument types.

    std::cout << "--- Solution 2:\n";

    naive_do_care(name = "naive", value = 2);
    naive_do_care(value = 2, name = "naive");

    std::cout << "--- Solution 3:\n";

    tuple_do_care(name = "tuple", value = 3);
    tuple_do_care(value = 3, name = "tuple");

    std::cout << "--- Solution 4:\n";

    tuple2_do_care(name = "tuple", value = 4);
    tuple2_do_care(value = 4, name = "tuple");

    using a_name = std::string_view;

    // Prefered way for named arguments
    tuple2_do_care(a_name() = "prefered");

    // But best ofcourse is to the traditional way!
    // tuple2_do_care(a_name("best"));
}

