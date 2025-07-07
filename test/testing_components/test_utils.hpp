#pragma once
#include <algorithm>
#include <array>

#include "../../include/utils/cxpr_stream.hpp"
#include "../../include/utils/enum_flag.hpp"

namespace circus::testing::utilities {
constexpr static std::string_view s1 = "hi";
constexpr static std::string_view s2 = "bob";
constexpr static std::string_view s3 = "bob";

static_assert(circus::utils::cxpr_stream<s1, s2, s3>::size == 8);
static_assert(circus::utils::cxpr_stream<s1, s2, s3>::value == "hibobbob");

enum class E : std::uint32_t {
    UNKNOWN = 1 << 0,
    FOO = 1 << 1,
    BAR = 1 << 2,
    CAR = 1 << 3
};

void test_enum_flag() {
    using ft = circus::utils::enum_flag<E>;
    ft flagger;

    flagger = (ft::enum_type::UNKNOWN | ft::enum_type::FOO);
    std::cout << flagger.has_any(ft::enum_type::BAR, ft::enum_type::CAR)
              << std::endl;
};

}  // namespace circus::testing::utilities