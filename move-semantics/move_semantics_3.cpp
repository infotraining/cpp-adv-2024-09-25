#include "helpers.hpp"

#include <catch2/catch_test_macros.hpp>
#include <iostream>

////////////////////////////////////////////////////////////////////////////
// Data - class with copy & move semantics (user provided implementation)

using namespace Helpers;

class Data
{
    std::string name_;
    int* data_;
    size_t size_;

public:
    using iterator = int*;
    using const_iterator = const int*;

    Data(std::string name, std::initializer_list<int> list)
        : name_{std::move(name)}
        , size_{list.size()}
    {
        data_ = new int[list.size()];
        std::copy(list.begin(), list.end(), data_);

        std::cout << "Data(" << name_ << ")\n";
    }

    // copy constructor
    Data(const Data& other)
        : name_(other.name_)
        , size_(other.size_)
    {
        std::cout << "Data(" << name_ << ": cc)\n";
        data_ = new int[size_];
        std::copy(other.begin(), other.end(), data_);
    }

    // copy copy assignment
    Data& operator=(const Data& other)
    {
        Data temp(other);
        swap(temp);

        std::cout << "Data=(" << name_ << ": cc)\n";

        return *this;
    }

    // move constructor
    Data(Data&& other) noexcept : name_(std::move(other.name_)), data_(other.data_), size_(other.size_)
    {
        other.data_ = nullptr;
        other.size_ = 0;

        std::cout << "Data(" << name_ << ": mv)\n";
    }

    // move assignment
    Data& operator=(Data&& other) noexcept
    {
        if (this != &other)
        {
            delete[] data_;

            // transfer of name_
            name_ = std::move(other.name_);
            
            // transfer of data_
            data_ = other.data_;
            other.data_ = nullptr;
            
            // transfer of size_
            size_ = other.size_;
            other.size_ = 0;
        }

        std::cout << "Data=(" << name_ << ": mv)\n";

        return *this;
    }

    ~Data() noexcept
    {
        delete[] data_;
    }

    void swap(Data& other)
    {
        name_.swap(other.name_);
        std::swap(data_, other.data_);
        std::swap(size_, other.size_);
    }

    iterator begin() noexcept
    {
        return data_;
    }

    iterator end() noexcept
    {
        return data_ + size_;
    }

    const_iterator begin() const noexcept
    {
        return data_;
    }

    const_iterator end() const noexcept
    {
        return data_ + size_;
    }
};

Data create_data_set()
{
    static int id_gen = 0;
    const int id = ++id_gen;

    Data ds{"Data#" + std::to_string(id), {54, 6, 34, 235, 64356, 235, 23}};

    return ds;
}

TEST_CASE("Data & move semantics")
{
    Data ds1{"ds1", {1, 2, 3, 4, 5}};

    Data backup = ds1; // copy
    Helpers::print(backup, "backup");

    Data target = std::move(ds1);
    Helpers::print(target, "target");

    Data ds2 = create_data_set();
    Helpers::print(ds2, "ds2");
}

namespace RuleOfFive
{
    struct DataSet
    {
        size_t id_;
        std::string name_;
        Data data_;

        DataSet(size_t id, const std::string& name, const Data& data) : id_(id), name_(name), data_(data)
        {}

        DataSet(const DataSet& other) = default;
        DataSet& operator=(const DataSet&) = default;
        DataSet(DataSet&& other) = default;
        DataSet& operator=(DataSet&& other) = default;

        ~DataSet() {}
    };
}

namespace RuleOfZero
{
    struct DataSet
    {
        size_t id_;
        std::string name_;
        Data data_;

        // DataSet(size_t id, const std::string& name, const Data& data) : id_(id), name_(name), data_(data)
        // {}

        // DataSet(size_t id, const std::string& name, Data&& data) : id_(id), name_(name), data_(std::move(data))
        // {}

        // DataSet(size_t id, std::string&& name, const Data& data) : id_(id), name_(std::move(name)), data_(data)
        // {}

        // DataSet(size_t id, std::string&& name, Data&& data) : id_(id), name_(std::move(name)), data_(std::move(data))
        // {}

        DataSet(size_t id, std::string name, Data data) : id_(id), name_(std::move(name)), data_(std::move(data))
        {}
    };
}

TEST_CASE("default special functions")
{
    using namespace RuleOfZero;

    DataSet ds1(665, "DataSet#1", Data("ds1", {1, 2, 3}));
    std::cout << "----\n";
    DataSet ds2 = ds1;
    std::cout << "----\n";
    DataSet ds3 = std::move(ds2);

    Data arg_ds{"arg_ds", {74, 6, 456, 56}};
    DataSet ds4(999, "DataSet#2", arg_ds);
}

void foo(int arg) noexcept
{}

void bar(int arg)
{
}

static_assert(!std::is_same_v<decltype(foo), decltype(bar)>);

TEST_CASE("noexcept")
{
    std::vector<Data> vec;
    vec.push_back(create_data_set());

    for(int i = 0; i < 10; ++i)
    {
        std::cout << "-------------\n";
        vec.push_back(create_data_set());
    }

    void (*ptr_fun1)(int) = foo;   
    // void (*ptr_fun2)(int) noexcept = bar; // ERROR
}