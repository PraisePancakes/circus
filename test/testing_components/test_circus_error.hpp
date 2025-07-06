#pragma once
#include <fstream>
#include <iostream>

#include "../../include/circus_error.hpp"

namespace circus::testing::semantic {
enum class ERROR_TYPES : std::uint32_t {
    ERROR_UNKNOWN = 1 << 0,
    ERROR_FOO = 1 << 1,
    ERROR_BAR = 1 << 2,
    ERROR_A = 1 << 3
};
void test_circus_error() {
    using rt = circus::error::reporter<ERROR_TYPES>;
    rt reporter;

    rt::type a = rt::type::ERROR_A;
    rt::type b = rt::type::ERROR_BAR;
    reporter |= a;
    reporter |= b;

    reporter = (a | b);
};
};  // namespace circus::testing::semantic