
#include <doctest/doctest.h>

#include "../../include/circus.hpp"
TEST_CASE("circus::lexer__ basic tokenization") {
    using namespace circus;

    lexer__ lexer;
    auto tokens = lexer("$testing 123.324, \"and a string\" \n\n{}");
    using TK = tokens__::TYPE;
    CHECK(!tokens.empty());

    REQUIRE(tokens.size() >= 7);

    CHECK(tokens[0]._token_type == TK::TK_DOLLA);
    CHECK(tokens[1]._token_type == TK::TK_IDENTIFIER);
    CHECK(tokens[1].embedded == "testing");
    bool is_float_or_double = tokens[2]._token_type == TK::TK_LITERAL_FLOAT || tokens[2]._token_type == TK::TK_LITERAL_DOUBLE;
    CHECK(is_float_or_double);
    CHECK(tokens[3]._token_type == TK::TK_COMMA);
    CHECK(tokens[4]._token_type == TK::TK_LITERAL_STRING);
    CHECK(tokens[4].embedded == std::string("\"and a string\""));

    CHECK(tokens[5]._token_type == TK::TK_CURL_L);
    CHECK(tokens[6]._token_type == TK::TK_CURL_R);
}