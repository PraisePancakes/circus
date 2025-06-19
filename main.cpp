#include <iostream>
#include "src/lexer.hpp"
#include "src/reader.hpp"

int main()
{

    circus::lexer__ lexer;
    lexer(std::filesystem::path("../example/example.txt"));

    return 0;
}