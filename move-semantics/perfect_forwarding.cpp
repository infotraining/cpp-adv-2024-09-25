#include "gadget.hpp"

#include <catch2/catch_test_macros.hpp>
#include <deque>

#ifdef _MSC_VER
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

using namespace Helpers;

////////////////////////////////////////////////////////
///  PERFECT FORWARDING

void have_fun(Gadget& g)
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
    g.use();
}

void have_fun(const Gadget& cg)
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
    cg.use();
}

void have_fun(Gadget&& g)
{
    std::cout << __PRETTY_FUNCTION__ << "\n";
    g.use();
}

namespace WithoutPerfectForwarding
{
    void use(Gadget& g)
    {
        std::cout << __PRETTY_FUNCTION__ << "\n";
        have_fun(g);
    }

    void use(const Gadget& g)
    {
        std::cout << __PRETTY_FUNCTION__ << "\n";
        have_fun(g);
    }

    void use(Gadget&& g)
    {
        std::cout << __PRETTY_FUNCTION__ << "\n";
        have_fun(std::move(g));
    }
} // namespace WithoutPerfectForwarding

namespace WithPerfectForwarding 
{
    template <typename TGadget>
    void use(TGadget&& g)
    {
        std::cout << __PRETTY_FUNCTION__ << "\n";       
        have_fun(std::forward<TGadget>(g));
    }
}

TEST_CASE("using gadget")
{
    Gadget g{1, "g"};
    const Gadget cg{2, "const g"};

    using WithPerfectForwarding::use;

    std::cout << "----\n";
    use(g);
    std::cout << "----\n";
    use(cg);
    std::cout << "----\n";
    use(Gadget{3, "temporary gadget"});
    std::cout << "----\n";
}