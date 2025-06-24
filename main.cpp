#include <iostream>
#include "include/lexer.hpp"
#include "include/reader.hpp"
#include "include/serializer.hpp"

int main()
{
    circus::serializer archive(std::cout);

    std::vector<int> v{0, 1, 2, 3, 4};

    archive(std::make_pair("V", v));
    return 0;
}