#include <iostream>
#include "src/lexer.hpp"
#include "src/reader.hpp"

int main()
{

    circus::lexer__ lexer;
    lexer("../example/example.txt");

    return 0;
}