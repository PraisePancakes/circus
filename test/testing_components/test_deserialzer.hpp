#pragma once
#include <fstream>
#include <iostream>

#include "../../include/deserializer.hpp"

namespace circus::testing::deserialization {

void test_deserializer() {
    std::ifstream ifs{"example/invalid_example.txt"};

    if (ifs.good()) {
        circus::deserializer dearch(ifs);
    }
};
};  // namespace circus::testing::deserialization