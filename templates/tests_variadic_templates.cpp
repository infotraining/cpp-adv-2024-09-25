#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <string>

void print()
{
    std::cout << "\n";
}

template <typename Head, typename... Tail>
void print(const Head& head, const Tail&... tail)
{
    std::cout << head << " ";

    print(tail...);
}

namespace SinceCpp17
{
    template <typename Head, typename... Tail>
    void print(const Head& head, const Tail&... tail)
    {
        std::cout << head << " ";

        if constexpr (sizeof...(tail) > 0)
            print(tail...);
        else
            std::cout << "\n";
    }
} // namespace SinceCpp17

/////////////////////////////////////////////////

template <typename... Types>
struct Count;

template <typename Head, typename... Tail>
struct Count<Head, Tail...>
{
    constexpr static int value = 1 + Count<Tail...>::value; // expansion pack
};

template <>
struct Count<>
{
    constexpr static int value = 0;
};

TEST_CASE("variadic templates")
{
    print(1, 3.14, "text", 665U);
    print("abc", std::string("def"));

    static_assert(Count<int, short, double>::value == 3);
}