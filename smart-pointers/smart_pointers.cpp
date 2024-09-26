#include "gadget.hpp"

#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <vector>
#include <map>

using Helpers::Gadget;

// https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#S-resource

namespace LegacyCode
{
    // forward declarations
    Gadget* get_gadget(const std::string& name);
    void use(Gadget* g);
    void use_gadget(Gadget* g);

    // definitions
    Gadget* get_gadget(const std::string& name)
    {
        static int id = 665;
        return new Gadget(++id, name);
    }

    void use(Gadget* g)
    {
        if (g)
            std::cout << "Using " << g->name() << "\n";

        delete g;
    }

    void use_gadget(Gadget* g)
    {
        if (g)
            std::cout << "Using " << g->name() << "\n";
    }

    void free_gadget(Gadget* g)
    {
        delete g;
    }
} // namespace LegacyCode

TEST_CASE("legacy hell with dynamic memory")
{
    using namespace LegacyCode;

    // {
    //     Gadget* g = get_gadget("ipad");

    //     use_gadget(g);
    // } // memory leak

    // {
    //     use_gadget(get_gadget("ipad"));
    // } // memory leak

    // {
    //     Gadget* g = new Gadget(13, "ipad");

    //     use(g);
    //     // use_gadget(g);  // UB!!! - use after delete
    //     // std::cout << g->name() << std::endl; // UB!!! - use after delete
    // }

    // {
    //     Gadget* g = get_gadget("ipad");

    //     use(g);

    //     delete g; // UB!!! - second delete
    // }
}

namespace ModernCpp
{
    std::unique_ptr<Gadget> get_gadget(const std::string& name);
    void use(std::unique_ptr<Gadget> g);

    std::unique_ptr<Gadget> get_gadget(const std::string& name)
    {
        static int id = 0;
        return std::make_unique<Gadget>(++id, name); // RVO
    }

    void use(std::unique_ptr<Gadget> g)
    {
        if (g)
            std::cout << "Using " << g->name() << "\n";
    }

    void use_gadget(Gadget* g)
    {
        if (g)
            std::cout << "Using " << g->name() << "\n";
    }

} // namespace ModernCpp

TEST_CASE("modern c++ with smart pointers")
{
    using namespace ModernCpp;

    {
        std::unique_ptr<Gadget> g = get_gadget("ipad");

        use_gadget(g.get());
    }

    {
        use(get_gadget("ipad"));
    }

    {
        auto g = std::make_unique<Gadget>(13, "ipad");

        use(std::move(g));
    }

    {
        std::unique_ptr<Gadget> g = get_gadget("ipad");

        use(std::move(g));
    }

    {
        std::unique_ptr<Gadget> g = get_gadget("ipad");

        LegacyCode::free_gadget(g.release());
    }
}

void may_throw()
{
    throw std::runtime_error("ERROR#14");
}

TEST_CASE("custom deallocators")
{
    SECTION("legacy")
    {
        FILE* fin = fopen("text.txt", "w+");

        fprintf(fin, "test");

        //may_throw();

        fclose(fin);      
    }

    SECTION("modern")
    {
        std::unique_ptr<FILE, int(*)(FILE*)> fin{fopen("text.txt", "w+"), fclose};

        fprintf(fin.get(), "test");

        //may_throw();
    }
}

namespace LegacyCode
{
    int* create_buffer()
    {
        return new int[1024];
    }
}

TEST_CASE("T[] specialization")
{
    std::unique_ptr<int[]> buffer{LegacyCode::create_buffer()};
    buffer[512] = 42;
}

TEST_CASE("shared_ptr")
{
    std::map<std::string, std::shared_ptr<Gadget>> gadgets;

    {
        auto g1 = std::make_shared<Gadget>(1, "ipad");
        REQUIRE(g1.use_count() == 1);
        
        auto g2 = g1;
        REQUIRE(g2.use_count() == 2);

        gadgets.emplace("ipad", g1);
        REQUIRE(g2.use_count() == 3);
    }

    gadgets["ipad"]->use();

    gadgets.clear();
}


struct Good : std::enable_shared_from_this<Good>
{
    std::string name;

    Good(std::string n) : name(std::move(n))
    {        
    }

    std::shared_ptr<Good> get_sptr()
    {
        return shared_from_this();
    }
};

TEST_CASE("enable_shared_from_this")
{
    auto ptr = std::make_shared<Good>("object");
    auto other_ptr = ptr->get_sptr();

    REQUIRE(ptr.use_count() == 2);
}

class Base
{
public:
    virtual void foo() {}
    virtual ~Base() = default;
};

class Derived : public Base
{
public:
    virtual void bar() {}
};


TEST_CASE("casts for shared_ptrs")
{
    std::shared_ptr<Base> ptr_base = std::make_shared<Derived>();
    
    std::shared_ptr<Derived> ptr_derived = std::dynamic_pointer_cast<Derived>(ptr_base);
    if (ptr_derived)
        ptr_derived->bar();

    REQUIRE(ptr_base.use_count() == 2);
}