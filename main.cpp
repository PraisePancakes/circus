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
        ar(CIRCUS_ENTRY(x), y, v);
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