
#include <doctest/doctest.h>

#include <fstream>
#include <sstream>

#include "../../include/lexer.hpp"
#include "../../include/parser.hpp"  // Adjust if path differs
#include "../../include/reader.hpp"

TEST_CASE("circus::parser__ invalid source input") {
    using namespace circus;

    std::ifstream ifs{"example/invalid_example.txt"};
    REQUIRE(ifs.good());

    CHECK_THROWS(
        []() {
            std::ifstream ifs{"example/invalid_example.txt"};
            std::string source = circus::filesystem::reader__{}(ifs);
            std::vector<circus::tokens__> tokens = circus::lexer__{}(std::move(source));
            std::unordered_map<std::string, circ_variable> rt = circus::parser__{}(std::move(tokens));
        }());
}