#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <array>

namespace Cpp11
{
    constexpr int factorial(int n)
    {
        return (n == 0) ? 1 : n * factorial(n - 1);
    }
} // namespace Cpp11

constexpr int fibonacci(int n)
{
    if (n == 0 || n == 1)
    {
        return n;
    }

    return fibonacci(n-1) + fibonacci(n-2); 
}

TEST_CASE("constexpr")
{
    constexpr int fact_4 = Cpp11::factorial(4);
    constexpr int fib_5 = fibonacci(5);

    int runtime_n = 10;
    std::cout << fibonacci(runtime_n) << "\n";
}

template <size_t N, typename F>
constexpr auto create_lookup_table(F f)
{
    std::array<int, N> lookup_table = {};

    for(int i = 0; i < N; ++i)
        lookup_table[i] = f(i);
    
    return lookup_table;
}

TEST_CASE("lookup table")
{
    constexpr auto fibonacci_lookup_table = create_lookup_table<20>(fibonacci);
}