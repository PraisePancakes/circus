#include <iostream>
#include "src/lexer.hpp"
#include "src/reader.hpp"
#include "src/archive.hpp"

int main()
{
    circus::serializer archive(std::cout);
    static_assert(circus::traits::StreamableVector<std::vector<int, std::allocator<int>>>);

    std::vector<int> v{0, 1, 2, 3, 4};

    archive(std::make_pair("V", v));
    return 0;
}