#pragma once
#include <fstream>
#include <iostream>
#include <vector>

#include "../../include/serializer.hpp"

namespace circus::testing::serialization {
namespace private_subjects {
struct C {
    int x = 0;
    int y = 0;

    C() {};
    template <typename Ar>
    void serialize(Ar &ar) {
        ar(CIRCUS_ENTRY(x), y);
    }
    ~C() {};
};

struct T {
    int x = 2;
    int y = 4;
    const std::string v = "hi";
    C c;
    T() {};
    template <typename Ar>
    void serialize(Ar &ar) {
        ar(CIRCUS_ENTRY(x), y, v, c);
    }

    ~T() {};
};

}  // namespace private_subjects

void test_serializer() {
    private_subjects::T t;
    int x = 3;
    std::vector<int> vec{0, 1, 2, 3, 4};
    std::ofstream of{"example/valid_example.txt"};
    if (of.good()) {
        circus::serializer archive(of);
        archive(CIRCUS_ENTRY(t), CIRCUS_ENTRY(x), CIRCUS_ENTRY(vec));
    } else {
        std::cout << "not good";
    }
};
};  // namespace circus::testing::serialization