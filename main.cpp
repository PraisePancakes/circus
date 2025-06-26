#include <iostream>
#include <vector>
#include "include/serializer.hpp"

struct T
{
    int x = 2;
    int y = 4;
    const std::string v = "hi";
    T() {};
    template <typename Ar>
    void serialize(Ar &ar)
    {
        ar(CIRCUS_ENTRY(x), CIRCUS_ENTRY(y), CIRCUS_ENTRY(v));
    }

    ~T() {};
};

int main()
{

    T t;
    circus::serializer archive(std::cout);
    archive(CIRCUS_ENTRY(t));
    return 0;
}