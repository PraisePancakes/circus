#pragma once
#include "../../include/lexer.hpp"
namespace circus::testing::semantics
{
    void test_lexer()
    {
        circus::lexer__ lexer;
        (void)lexer("$testing 123.324, \"and a string\" \n\n{}");
    };
};