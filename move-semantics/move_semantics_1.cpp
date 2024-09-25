#include "helpers.hpp"

#include <catch2/catch_test_macros.hpp>
#include <iostream>

using namespace std::literals;

std::string full_name(const std::string& first_name, const std::string& last_name)
{
    return first_name + " " + last_name;
}

TEST_CASE("reference binding")
{
    std::string name = "jan";

    SECTION("C++98")
    {
        std::string& lvref_name = name;
        const std::string& ref_full_name = full_name(name, "Kowalski");
        // ref_full_name[0] = 'J';
    }

    SECTION("C++11")
    {
        std::string&& rvref_full_name = full_name(name, "Kowalski");
        rvref_full_name[0] = 'J';
        REQUIRE(rvref_full_name == "Jan Kowalski");

        // std::string&& rvref_name = name; // ERROR - You cannot bind an lvalue to an rvalue reference
    }
}

class Vector
{
public:
    void push_back(const std::string& s)
    {
        std::cout << "push_back(const string&) - makes copy of " << s << " to vector\n";
    }

    void push_back(std::string&& s)
    {
        std::cout << "push_back(string&&) - moves " << s << " to vector\n";
    }
};

TEST_CASE("function overloading with references")
{
    std::string str = "text";

    Vector vec;

    vec.push_back(str);
    vec.push_back(str + str);
    vec.push_back("ctext");
    vec.push_back(static_cast<std::string&&>(str));
    vec.push_back(str);
}