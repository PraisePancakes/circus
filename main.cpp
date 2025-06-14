#include <iostream>
#include "src/lexer.hpp"
#include "src/reader.hpp"

int main(int argc, char **argv)
{

    circus::lexer__ l("../example/example.txt");
    std::cout << l.get_input() << std::endl;

    return 0;
}