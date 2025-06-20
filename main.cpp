#include <iostream>
#include "src/lexer.hpp"
#include "src/reader.hpp"
#include "src/archive.hpp"

int main()
{
    circus::text_archive("../example/example.txt");
    return 0;
}