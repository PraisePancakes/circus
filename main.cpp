#include <iostream>
#include <vector>

#include "include/serializer.hpp"
int main()
{

    circus::serializer archive(std::cout);

    std::vector<int> v{0, 1, 2, 5, 4};

    archive(std::make_pair("V", v));
    return 0;
}