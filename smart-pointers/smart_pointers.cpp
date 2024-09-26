#include "gadget.hpp"

#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <vector>

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

    {
        Gadget* g = get_gadget("ipad");

        use_gadget(g);
    } // memory leak

    {
        use_gadget(get_gadget("ipad"));
    } // memory leak

    {
        Gadget* g = new Gadget(13, "ipad");

        use(g);
        // use_gadget(g);  // UB!!! - use after delete
        // std::cout << g->name() << std::endl; // UB!!! - use after delete
    }

    {
        Gadget* g = get_gadget("ipad");

        use(g);

        delete g; // UB!!! - second delete
    }
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
} // delete[]