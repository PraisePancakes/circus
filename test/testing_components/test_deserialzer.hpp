#pragma once
#include <fstream>
#include <iostream>

#include "../../include/deserializer.hpp"

namespace circus::testing::deserialization {

void test_deserializer() {
    std::ifstream ifs{"example/valid_example.txt"};

    if (ifs.good()) {
        circus::deserializer dearch(ifs);
        int v = dearch["root"]["x"];
        std::string v2 = dearch["root"]["y"];
        std::cout << v << std::endl;
        std::cout << v2 << std::endl;
    }
};
};  // namespace circus::testing::deserialization