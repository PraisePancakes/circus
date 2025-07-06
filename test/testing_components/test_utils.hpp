#pragma once
#include <algorithm>
#include <array>

#include "../../include/utils.hpp"

namespace circus::testing::utilities {
constexpr static std::string_view s1 = "hi";
constexpr static std::string_view s2 = "bob";
constexpr static std::string_view s3 = "bob";

static_assert(circus::utils::cxpr_stream<s1, s2, s3>::size == 8);
static_assert(circus::utils::cxpr_stream<s1, s2, s3>::value == "hibobbob");
}  // namespace circus::testing::utilities