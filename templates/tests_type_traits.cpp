#include <catch2/catch_test_macros.hpp>
#include <type_traits>

int foo(int x)
{
    return 42 * x;
}

template <int X>
struct Foo
{
    constexpr static int value = 42 * X;
};

// template  variable
template <int X>
constexpr static int Foo_v = Foo<X>::value;

////////////////////////////////////////////////

template <typename T, T Value>
struct IntegralConstant
{
    constexpr static T value = Value;
};

template <bool Value>
using BoolConstant = IntegralConstant<bool, Value>;

using TrueType = BoolConstant<true>;
using FalseType = BoolConstant<false>;

static_assert(TrueType::value == true);
static_assert(FalseType::value == false);

////////////////////////////////////////////////

template <typename T>
struct Identity
{
    using type = T;
};

// template  alias
template <typename T>
using Identity_t = typename Identity<T>::type;

////////////////////////////////////////////////

template <typename T1, typename T2>
struct IsSame : FalseType
{};

template <typename T>
struct IsSame<T, T> : TrueType
{};

template <typename T1, typename T2>
constexpr static bool IsSame_v = IsSame<T1, T2>::value;

////////////////////////////////////////////////

template <typename T>
struct IsPointer : FalseType
{};

template <typename T>
struct IsPointer<T*> : TrueType
{};

template <typename T>
constexpr bool IsPointer_v = IsPointer<T>::value;

TEST_CASE("type traits")
{   
    REQUIRE(foo(2) == 84);
    static_assert(Foo<2>::value == 84);
    static_assert(Foo_v<2> == 84);

    static_assert(IntegralConstant<int, 42>::value == 42);

    Identity<int>::type x = 42;
    Identity_t<double> y = 3.24;

    using Str = std::string;
    static_assert(!IsSame<Identity_t<int>, Str>::value);
    static_assert(IsSame_v<Identity_t<int>, int>);
    static_assert(std::is_same_v<Identity_t<std::string>, Str>);

    static_assert(IsPointer<int>::value == false);
    static_assert(IsPointer<int*>::value == true);
    static_assert(IsPointer_v<const std::string*> == true);
}