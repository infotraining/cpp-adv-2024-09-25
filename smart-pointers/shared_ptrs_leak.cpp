#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <memory>

class Human
{
public:
    Human(const std::string& name)
        : name_(name)
    {
        std::cout << "Constructor Human(" << name_ << ")" << std::endl;
    }

    Human(const Human&) = delete;
    Human& operator=(const Human&) = delete;

    ~Human()
    {
        std::cout << "Destructor ~Human(" << name_ << ")" << std::endl;
    }

    void set_partner(std::shared_ptr<Human> partner)
    {
        partner_ = partner;
    }

    void description() const
    {
        std::cout << "My name is " << name_ << std::endl;

        std::shared_ptr<Human> living_partner = partner_.lock();
        if (living_partner)
        {
            std::cout << "My partner is " << living_partner->name_ << std::endl;
        }
    }

private:
    std::weak_ptr<Human> partner_;
    std::string name_;
};

TEST_CASE("shared_ptrs leak - circular dependency")
{
    // RC partner1 == 1
    auto partner1 = std::make_shared<Human>("Jan");

    // RC partner2 == 1
    auto partner2 = std::make_shared<Human>("Ewa");

    // RC partner2 ==2
    partner1->set_partner(partner2);

    // RC partner2 == 2
    partner2->set_partner(partner1);

    partner1->description();
}
