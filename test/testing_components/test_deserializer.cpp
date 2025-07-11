
#include <doctest/doctest.h>

#include <fstream>
#include <sstream>

#include "../../include/deserializer.hpp"

TEST_CASE("circus::deserializer valid source input") {
    using namespace circus;
    std::ifstream ifs{"example/valid_example.txt"};
    REQUIRE(ifs.good());
    deserializer dearch(ifs);
    CHECK(dearch["root"]["x"].value<int>() == 3);
    CHECK(dearch["root"]["y"].value<std::string>() == "\"hi\"");
}
