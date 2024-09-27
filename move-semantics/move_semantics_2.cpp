#include "gadget.hpp"

#include <catch2/catch_test_macros.hpp>
#include <vector>

////////////////////////////////////////////////
// simplified implementation of unique_ptr - only moveable type

namespace Explain
{
    template <typename T>
    class UniquePtr
    {
        T* object_ptr;

    public:
        explicit UniquePtr(T* object) noexcept
            : object_ptr{object}
        {
        }

        UniquePtr(const UniquePtr& other) = delete;
        UniquePtr& operator=(const UniquePtr& other) = delete;

        // move constructor
        UniquePtr(UniquePtr&& other) noexcept
            : object_ptr{other.object_ptr}
        {
            other.object_ptr = nullptr;
        }

        // move assignment operator
        UniquePtr& operator=(UniquePtr&& other) noexcept
        {
            if (this != &other)
            {
                delete object_ptr; // release of previous state

                object_ptr = other.object_ptr;
                other.object_ptr = nullptr;
            }

            return *this;
        }

        ~UniquePtr() noexcept
        {
            delete object_ptr;
        }

        explicit operator bool() const noexcept
        {
            return object_ptr != nullptr;
        }

        T& operator*() const noexcept
        {
            return *object_ptr;
        }

        T* operator->() const noexcept
        {
            return object_ptr;
        }

        T* get() const noexcept
        {
            return object_ptr;
        }
    };
} // namespace Explain

Explain::UniquePtr<Helpers::Gadget> create_gadget()
{
    using Helpers::Gadget;
    static int id_gen = 0;
    const int id = ++id_gen;
    return Explain::UniquePtr<Gadget>{new Gadget{id, "Gadget#" + std::to_string(id)}};
}

namespace Explain
{
    // template <typename T>
    // UniquePtr<T> make_unique()
    // {
    //     return UniquePtr<T>{new T()};
    // }

    // template <typename T, typename TArg1>
    // UniquePtr<T> make_unique(TArg1&& arg1)
    // {
    //     return UniquePtr<T>{new T(std::forward<TArg1>(arg1))};
    // }

    // template <typename T, typename TArg1, typename TArg2>
    // UniquePtr<T> make_unique(TArg1&& arg1, TArg2&& arg2)
    // {
    //     return UniquePtr<T>{new T(std::forward<TArg1>(arg1), std::forward<TArg2>(arg2))};
    // }

    template <typename T, typename... TArgs>
    UniquePtr<T> make_unique(TArgs&&... args)
    {
        return UniquePtr<T>{new T(std::forward<TArgs>(args)...)};
    }
}

TEST_CASE("move semantics - unique_ptr")
{
    using Explain::UniquePtr;
    using Helpers::Gadget;

    auto ptr0 = Explain::make_unique<Gadget>();

    //UniquePtr<Gadget> ptr1{new Gadget(1, "ipad")};
    UniquePtr<Gadget> ptr1 = Explain::make_unique<Gadget>(1, "ipad");
    if (ptr1)
    {
        (*ptr1).use();
        ptr1->use();
    }

    UniquePtr<Gadget> ptr2 = std::move(ptr1); // call of move constructor
    if (ptr2)
        ptr2->use();
    assert(ptr1.get() == nullptr);

    UniquePtr<Gadget> ptr3 = Explain::make_unique<Gadget>(42, "smartwatch");
    ptr3->use();

    ptr3 = std::move(ptr2); // call of move assignment operator
    ptr3->use();

    ptr3 = std::move(ptr3); // self assignment

    std::vector<UniquePtr<Gadget>> gadgets;
    gadgets.push_back(std::move(ptr3));
    gadgets.push_back(create_gadget());
    gadgets.push_back(create_gadget());
    gadgets.push_back(create_gadget());
    gadgets.push_back(create_gadget());

    for (const auto& g : gadgets)
    {
        if (g)
            g->use();
    }

    std::vector<Gadget> gs;
    gs.push_back(Gadget{1, "ipad"});
    gs.emplace_back(42, "ipad");
}

TEST_CASE("std::move")
{
    SECTION("non-class/struct types")
    {
        int x = 42;
        int* ptr1 = &x;

        int y = std::move(x);  // copy
        REQUIRE(x == y);

        int* ptr2 = std::move(ptr1); // copy
        REQUIRE(ptr1 == ptr2);
    }

    SECTION("class/struct")
    {
        std::string str = "text";

        std::string target = std::move(str); // move constructor of string
        REQUIRE(target == "text");
        REQUIRE(str.size() == 0);
    }
}