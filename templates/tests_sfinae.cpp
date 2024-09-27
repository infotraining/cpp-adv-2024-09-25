#include <catch2/catch_test_macros.hpp>

template <bool Condition, typename T = void>
struct EnableIf
{
    using type = T;
};

template <typename T>
struct EnableIf<false, T>
{ };

template <bool Condition, typename T = void>
using EnableIf_t = typename EnableIf<Condition, T>::type;

///////////////////////////////////////////////////////////////

template <typename T>
auto is_power_of_2(T value) -> std::enable_if_t<std::is_integral_v<T>, bool>
{
    return value > 0 && (value & (value - 1)) == 0;
}

template <typename T>
auto is_power_of_2(T value) -> std::enable_if_t<std::is_floating_point_v<T>, bool>
{
    int exponent;
    const T mantissa = std::frexp(value, &exponent);
    return mantissa == T(0.5);
}

namespace SinceCpp17
{
    template <typename T>
    auto is_power_of_2(T value)
    {
        if constexpr (std::is_integral_v<T>)
        {
            return value > 0 && (value & (value - 1)) == 0;
        }
        else
        {
            int exponent;
            const T mantissa = std::frexp(value, &exponent);
            return mantissa == T(0.5);
        }
    }

} // namespace SinceCpp17

TEST_CASE("SFINAE")
{
    REQUIRE(is_power_of_2(8));
    REQUIRE(is_power_of_2(64));
    REQUIRE(is_power_of_2(256));

    REQUIRE(is_power_of_2(8.0));
    REQUIRE(SinceCpp17::is_power_of_2(256.0f));
}