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

namespace HeadTail
{
    template <typename Head, typename... Tail>
    auto sum(const Head& head, const Tail&... tail)
    {
        if constexpr (sizeof...(tail) > 0)
            return head + sum(tail...);
        else
            return head;
    }
} // namespace HeadTail

inline namespace Fold
{
    template <typename... TValues>
    auto sum(TValues... values) // (1, 2, 3, 4, 5)
    {
        return (... + values); // fold expression for +
        // return ((((1 + 2) + 3) + 4) + 5);
    }
}

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

// TODO

TEST_CASE("sum")
{
    REQUIRE(sum(1, 2, 3, 4) == 10);
    REQUIRE(sum(1, 2, 3, 4, 5) == 15);
}

template <typename F, typename... TArgs>
void call_for_all(F f, TArgs&&... args)
{
    (..., f(std::forward<TArgs>(args))); // fold for ,
}

TEST_CASE("call_for_all")
{
    auto printer = [](const auto& item) { std::cout << "item: " << item << "\n"; };

    call_for_all(printer, 1, 3.14, "text");
}