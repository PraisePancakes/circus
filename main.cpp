#include <iostream>
#include "src/lexer.hpp"
#include "src/reader.hpp"
#include "src/archive.hpp"

int main()
{
    circus::serializer archive(std::cout);

    archive('a', 1, 2, 'b');
    return 0;
}