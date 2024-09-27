#include "helpers.hpp"

#include <catch2/catch_test_macros.hpp>
#include <deque>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <string_view>
#include <map>

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

template <typename T, typename TContainer = std::vector<T>>
class Stack
{
    TContainer items_;

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

template <typename T, typename TContainer>
template <typename TValue>
void Stack<T, TContainer>::push(TValue&& val)
{
    items_.push_back(std::forward<TValue>(val));
}

template <typename T, typename TContainer>
void Stack<T, TContainer>::pop()
{
    items_.pop_back();
}

namespace TemplateTemplateParameter
{
    template <typename T, template <typename, typename> class Container, typename TAllocator = std::allocator<T>>
    class Stack
    {
        Container<T, TAllocator> items_;

    public:
        Stack() = default;

        template <typename TValue>
        void push(TValue&& val)
        {
            items_.push_back(std::forward<TValue>(val));
        }

        T& top()
        {
            return items_.back();
        }

        const T& top() const
        {
            return items_.back();
        }

        void pop()
        {
            items_.pop_back();
        }

        size_t size() const
        {
            return items_.size();
        }
    };
} // namespace TemplateTemplateParameter

TEST_CASE("class templates")
{
    TemplateTemplateParameter::Stack<int, std::deque> stack_int;
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

namespace Specialization
{
    template <typename T>
    class Holder
    {
        T value_;
    public:
        Holder(T value) : value_(std::move(value))
        {
        }

        T& value()
        {
            return value_;
        }

        const T& value() const
        {
            return value_;
        }
    };

    template <typename T>
    class Holder<T*>  // partial specialization of Holder template
    {
        std::unique_ptr<T> ptr_;
    public:
        explicit Holder(T* ptr) : ptr_{ptr}
        {}

        T& value()
        {
            return *ptr_;
        }

        const T& value() const
        {
            return *ptr_;
        }

        void reset(T* new_ptr)
        {
            ptr_.reset(new_ptr);
        }
    };

    template <>
    class Holder<const char*> // full specialization for const char*
    {
        std::string_view value_;
    public:
        Holder(const char* cstr) : value_(cstr)
        {}

        std::string_view value() const
        {
            return value_;
        }
    };
}

TEST_CASE("Specialization - class templates")
{
    using namespace Specialization;

    Holder<int> v1{42};
    REQUIRE(v1.value() == 42);

    Holder<int*> v2{new int(665)};
    REQUIRE(v2.value() == 665);

    Holder<const char*> v3 = "text";
    REQUIRE(v3.value().size() == 4);
}

// TODO
namespace Explain
{
    // template <typename TContainer>
    // size_t size(const TContainer& con)
    // {
    //     // 1 - STL containers
    //     return con.size();

    //     // 2 - native arrays
    //     using TValue = decltype(con[0]); // ???
    //     return sizeof(con) / sizeof(TValue);
    // }

    template <typename TContainer>
    size_t size(const TContainer& con)
    {
        return con.size();
    }

    template <typename T, size_t N>
    size_t size(T (&tab)[N])
    {
        return N;
    }
} // namespace Explain

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

        double dbls[10];
        static_assert(std::is_same_v<decltype(dbls[0]), double&>);
    }
}

template <typename T>
using Dictionary = std::map<std::string, T>;

template <size_t N>
using Buffer = Array<std::byte, N>;

TEST_CASE("template aliases")
{
    Dictionary<int> dict = { {"one", 1}, {"two", 2 } };
    Buffer<1024> buffer = {};
}

template<typename T>
constexpr T pi{3.1415926535897932385};

TEST_CASE("template variables")
{
    std::cout << "pi<double> = " << pi<double> << "\n";
    std::cout << "pi<float> = " << pi<float> << "\n";

    static_assert(sizeof(pi<double>) != sizeof(pi<float>));
}