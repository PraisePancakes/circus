#include <iostream>
#include "src/lexer.hpp"
#include "src/reader.hpp"

int main()
{

    circus::lexer__ lexer;
    (void)lexer(std::filesystem::path("../example/example.txt"));

    return 0;
}