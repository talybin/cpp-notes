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


// Using if constexpr
template <typename... Args>
void if_constexpr_read_args(const Args&... args)
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


// Require that all used arguments specified.
// No check for other types except duplicates.
template <typename... Args>
void tuple_required_all(const Args&... args)
{
    std::tuple<const Args&...> tuple_args(args...);

    std::cout << std::get<const std::string_view&>(tuple_args)
              << " number is " << std::get<const int&>(tuple_args) << '\n';
}


// Has default values, don't require all arguments and
// detect types not listed in "needed_args".
template <typename... Args>
void tuple_optional_args(Args&&... args)
{
    // Accepted args with default values
    std::tuple<std::string_view, int> needed_args;

    // Copy/Move passed arguments
    auto set = [&needed_args]<typename T>(T&& arg) {
        std::get<std::decay_t<T>>(needed_args) = std::forward<T>(arg);
    };
    (set(std::forward<Args>(args)), ...);

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

    if_constexpr_read_args(name = "if_constexpr_read_args", value = 2);
    if_constexpr_read_args(value = 2, name = "if_constexpr_read_args");

    std::cout << "--- Solution 3:\n";

    tuple_required_all(name = "tuple_required_all", value = 3);
    tuple_required_all(value = 3, name = "tuple_required_all");

    std::cout << "--- Solution 4:\n";

    tuple_optional_args(name = "tuple_optional_args", value = 4);
    tuple_optional_args(value = 4, name = "tuple_optional_args");

    using a_name = std::string_view;

    // Prefered way for named arguments
    tuple_optional_args(a_name() = "prefered");

    // Best solution would of course be to not use
    // named arguments for clarity and speed of
    // the code. But you already know it, right? :)
    // Still, since we talking about named args,
    // I guess less painfull would be:
    tuple_optional_args(a_name("best"));
}

