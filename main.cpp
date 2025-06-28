#include <iostream>
#include <vector>
#include "include/serializer.hpp"

struct C
{
    int x = 0;
    int y = 0;

    C() {};
    template <typename Ar>
    void serialize(Ar &ar)
    {
        ar(CIRCUS_ENTRY(x), y);
    }
    ~C() {};
};

struct T
{
    int x = 2;
    int y = 4;
    const std::string v = "hi";
    C c;
    T() {};
    template <typename Ar>
    void serialize(Ar &ar)
    {
        ar(CIRCUS_ENTRY(x), y, v, c);
    }

    ~T() {};
};

int main()
{

    T t;
    std::vector<int> vec{0, 1, 2, 3, 4};
    circus::serializer archive(std::cout);
    archive(CIRCUS_ENTRY(t));
    return 0;
}