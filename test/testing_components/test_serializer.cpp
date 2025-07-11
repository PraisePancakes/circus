#include <doctest/doctest.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "../../include/circus.hpp"

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
    T(int x, int y) : x(x), y(y) {};
    template <typename Ar>
    void serialize(Ar &ar) {
        ar(CIRCUS_ENTRY(x), y, v, c);
    }
    ~T() {};
};
}  // namespace private_subjects

TEST_CASE("circus::serializer basic serialization operator()") {
    private_subjects::T t(5555, 5555);
    int x = 3;
    std::vector<int> vec{0, 1, 2, 3, 4};

    // Use a stringstream instead of a real file for test isolation
    std::ostringstream oss;

    circus::serializer archive(oss);
    archive(CIRCUS_ENTRY(t), CIRCUS_ENTRY(x), CIRCUS_ENTRY(vec));

    // The output should not be empty
    std::string serialized = oss.str();
    CHECK(!serialized.empty());

    // Optionally, check if some expected substrings appear
    CHECK(serialized.find("5555") != std::string::npos);
    CHECK(serialized.find("3") != std::string::npos);
    CHECK(serialized.find("0") != std::string::npos);
}

TEST_CASE("circus::serializer basic serialization operator<<") {
    private_subjects::T t(5555, 5555);
    int x = 3;
    std::vector<int> vec{0, 1, 2, 3, 4};

    // Use a stringstream instead of a real file for test isolation
    std::ostringstream oss;

    circus::serializer archive(oss);
    archive << CIRCUS_ENTRY(t) << CIRCUS_ENTRY(x) << CIRCUS_ENTRY(vec);

    // The output should not be empty
    std::string serialized = oss.str();
    CHECK(!serialized.empty());

    // Optionally, check if some expected substrings appear
    CHECK(serialized.find("5555") != std::string::npos);
    CHECK(serialized.find("3") != std::string::npos);
    CHECK(serialized.find("0") != std::string::npos);
}
}  // namespace circus::testing::serialization
