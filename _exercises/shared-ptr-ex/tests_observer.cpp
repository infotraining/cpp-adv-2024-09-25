#include <cassert>
#include <catch2/catch_test_macros.hpp>
#include <cstdlib>
#include <iostream>
#include <set>
#include <stdexcept>
#include <string>

class Observer
{
public:
    virtual void update(const std::string& event_args) = 0;
    virtual ~Observer() = default;
};

namespace Ver_1
{
    class Subject
    {
        int state_;
        std::set<Observer*> observers_;

    public:
        Subject()
            : state_(0)
        {
        }

        void register_observer(Observer* observer)
        {
            observers_.insert(observer);
        }

        void unregister_observer(Observer* observer)
        {
            observers_.erase(observer);
        }

        void set_state(int new_state)
        {
            if (state_ != new_state)
            {
                state_ = new_state;
                notify("Changed state on: " + std::to_string(state_));
            }
        }

    protected:
        void notify(const std::string& event_args)
        {
            for (Observer* observer : observers_)
            {
                observer->update(event_args);
            }
        }
    };
} // namespace Ver_1

namespace Ver_2
{
    class Subject
    {
        int state_;
        std::set<std::weak_ptr<Observer>, std::owner_less<std::weak_ptr<Observer>>> observers_;

    public:
        Subject()
            : state_(0)
        {
        }

        void register_observer(std::weak_ptr<Observer> observer)
        {
            observers_.insert(observer);
        }

        void unregister_observer(std::weak_ptr<Observer> observer)
        {
            observers_.erase(observer);
        }

        void set_state(int new_state)
        {
            if (state_ != new_state)
            {
                state_ = new_state;
                notify("Changed state on: " + std::to_string(state_));
            }
        }

    protected:
        void notify(const std::string& event_args)
        {
            for (std::weak_ptr<Observer> observer : observers_)
            {
                if (std::shared_ptr<Observer> living_observer = observer.lock(); living_observer)
                    living_observer->update(event_args);
            }
        }
    };
} // namespace Ver_1

class ConcreteObserver1 : public Observer
{
public:
    virtual void update(const std::string& event)
    {
        std::cout << "ConcreteObserver1: " << event << std::endl;
    }
};

class ConcreteObserver2 : public Observer
{
public:
    virtual void update(const std::string& event)
    {
        std::cout << "ConcreteObserver2: " << event << std::endl;
    }
};

TEST_CASE("using observer pattern - ver 1")
{
    using namespace std;

    Ver_1::Subject s;

    auto deregister_observer = [&s](Observer* o) {
        s.unregister_observer(o);
        delete o;
    };
    using TDeregisterObserver = decltype(deregister_observer);

    std::unique_ptr<Observer, TDeregisterObserver> o1{new ConcreteObserver1(), deregister_observer};
    s.register_observer(o1.get());

    {
        // ConcreteObserver2* o2 = new ConcreteObserver2;
        std::shared_ptr<Observer> o2{new ConcreteObserver2(), deregister_observer};
        s.register_observer(o2.get());

        s.set_state(1);

        o2.reset();

        cout << "End of scope." << endl;
    }

    s.set_state(2);
}

TEST_CASE("using observer pattern - ver 2")
{
    using namespace std;

    Ver_2::Subject s;

    std::shared_ptr<Observer> o1 = std::make_shared<ConcreteObserver1>();
    s.register_observer(o1);

    {
        std::shared_ptr<Observer> o2 = std::make_shared<ConcreteObserver2>();

        s.register_observer(o2);

        s.set_state(1);

        o2.reset();

        cout << "End of scope." << endl;
    }

    s.set_state(2);
}
