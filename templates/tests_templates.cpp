#include "helpers.hpp"

#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

using Helpers::Gadget;
using namespace std::literals;

template <typename T>
auto maximum(T a, T b)
{
    return (a < b) ? b : a;
}

template <>
auto maximum<const char*>(const char* a, const char* b)
{
    return std::strcmp(a, b) < 0 ? b : a;
}

namespace ReturnTypeTrait
{
    template <typename T1, typename T2, typename TResult = std::common_type_t<T1, T2>>
    TResult maximum(T1 a, T2 b)
    {
        if (a < b)
            return b;
        return a;
    }
} // namespace ReturnTypeTrait

TEST_CASE("function templates")
{
    REQUIRE(maximum<int>(42, 665) == 665);
    REQUIRE(maximum(3.14, 6.28) == 6.28);

    std::string str1 = "ABC";
    std::string str2 = "ABD";
    REQUIRE(maximum(str1, str2) == "ABD");

    REQUIRE(maximum<int>(42, 3.14) == 42);
    REQUIRE(maximum(static_cast<double>(42), 3.14) == 42);

    const char* cstr1 = "ABF";
    const char* cstr2 = "ABD";
    REQUIRE(maximum<const char*>(cstr1, cstr2) == "ABF"s);

    REQUIRE(ReturnTypeTrait::maximum<int, double, long>(42, 42.4) == 42.4);
}

template <typename T>
class Stack
{
    std::vector<T> items_;

public:
    Stack() = default;

    template <typename TValue>
    void push(TValue&& val);

    T& top()
    {
        return items_.back();
    }

    const T& top() const
    {
        return items_.back();
    }

    void pop();

    size_t size() const
    {
        return items_.size();
    }
};

template <typename T>
template <typename TValue>
void Stack<T>::push(TValue&& val)
{
    items_.push_back(std::forward<TValue>(val));
}

template <typename T>
void Stack<T>::pop()
{
    items_.pop_back();
}

TEST_CASE("class templates")
{
    Stack<int> stack_int;
    stack_int.push(42);
    stack_int.push(665);
    REQUIRE(stack_int.size() == 2);

    int item = stack_int.top();
    stack_int.pop();

    REQUIRE(item == 665);
    REQUIRE(stack_int.size() == 1);
}

template <typename T1, typename T2>
struct ValuePair
{
    T1 first;
    T2 second;

    ValuePair(T1 fst, T2 snd)
        : first(fst)
        , second(snd)
    { }
};

TEST_CASE("CTAD - since C++17")
{
    ValuePair<int, double> vp1{10, 3.14};
    ValuePair vp2{10, 31.4}; // ValuePair<int, double>

    std::vector vec = {1, 2, 3, 4}; // std::vector<int>
}

template <typename T, size_t N>
struct Array
{
    T items_[N];

    using iterator = T*;
    using const_iterator = const T*;
    using reference = T&;
    typedef const T& const_reference;

    size_t size() const
    {
        return N;
    }

    iterator begin()
    {
        return items_;
    }

    iterator end()
    {
        return items_ + N;
    }

    const_iterator begin() const
    {
        return items_;
    }

    const_iterator end() const
    {
        return items_ + N;
    }
};

// TODO
namespace Explain
{
    template <typename TContainer>
    size_t size(const TContainer& con)
    {
        return con.size();
    }

    template <typename T, size_t N>
    size_t size(T(&tab)[N])
    {
        return N;
    }
}

TEST_CASE("Array - NTTP")
{
    SECTION("Array")
    {
        Array<int, 10> arr1 = {1, 2, 3, 4, 5};

        for (const auto& item : arr1)
        {
            std::cout << item << " ";
        }
        std::cout << "\n";
    }

    SECTION("size")
    {
        std::vector<int> vec = {1, 2, 3};
        REQUIRE(Explain::size(vec) == 3);

        int tab[1024] = {};
        REQUIRE(Explain::size(tab) == 1024);
    }
}

TEST_CASE("template aliases")
{
    // TODO
}

TEST_CASE("template variables")
{
    // TODO
}