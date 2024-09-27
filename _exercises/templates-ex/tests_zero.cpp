#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <list>
#include <string>
#include <vector>

namespace Exercise
{
    // TODO: Implement a function that sets all elements of a container to zero (default value).

    namespace Ver_1
    {

        template <typename TContainer>
        void zero(TContainer& x)
        {
            std::fill(x.begin(), x.end(), 0);
        }

        template <>
        void zero<std::list<std::string>>(std::list<std::string>& x)
        {
            std::fill(x.begin(), x.end(), "");
        }

    } // namespace Ver_1

    namespace Ver_2
    {
        template <template <typename, typename> class ContainerT, typename TAllocator = std::allocator<int>>
        void zero(ContainerT<int, TAllocator>& container)
        {
            for (int& element : container)
            {
                element = 0;
            }
        }

        template <template <typename, typename> class ContainerT, typename TAllocator = std::allocator<std::string>>
        void zero(ContainerT<std::string, TAllocator>& container)
        {
            for (std::string& element : container)
            {
                element = "";
            }
        }
    } // namespace Ver_2

    namespace Ver_3
    {
        template <typename TContainer>
        void zero(TContainer& container)
        {
            using TValue = typename TContainer::value_type;

            std::fill(std::begin(container), std::end(container), TValue{});
        }

        template <typename T, std::size_t N>
        void zero(T(&arr)[N])
        {
            for (std::size_t i = 0; i < N; ++i)
            {
                arr[i] = T{};
            }
        }
    } // namespace Ver_3

    inline namespace Ver_4
    {
        template <typename TContainer>
        void zero(TContainer& container)
        {
            using TValue = std::remove_reference_t<decltype(*std::begin(container))>;            

            std::fill(std::begin(container), std::end(container), TValue{});

            // memset - for native tables & POD types ???
        }
    } // namespace Ver_3


} // namespace Exercise

TEST_CASE("zero - Step 1")
{
    using namespace Exercise;

    SECTION("vector<int>")
    {
        std::vector<int> vec = {1, 2, 3, 4};

        zero(vec);

        REQUIRE(vec == std::vector{0, 0, 0, 0});
    }

    SECTION("list<std::string>")
    {
        std::list<std::string> lst = {"one", "two", "three"};

        zero(lst);

        REQUIRE(lst == std::list<std::string>{"", "", ""});
    }

    SECTION("vector<std::string>")
    {
        std::vector<std::string> lst = {"one", "two", "three"};

        zero(lst);

        REQUIRE(lst == std::vector<std::string>{"", "", ""});
    }

    SECTION("int[4]")
    {
        int tab[10] = {1, 2, 3, 4};
        
        zero(tab);

        REQUIRE(std::all_of(std::begin(tab), std::end(tab), [](int x) { return x == 0; }));
    }
}