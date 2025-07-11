/**
 * @file cxpr_stream.hpp
 * @brief Provides a compile-time stream concatenation utility using string_views.
 */

#pragma once
#include <algorithm>
#include <iostream>

namespace circus::utils {

/**
 * @brief Compile-time concatenation of multiple string_views into a single string_view.
 *
 * This class template accepts a pack of string_view references and
 * concatenates them into a single contiguous char array at compile time.
 *
 * @tparam args A parameter pack of const std::string_view& to be concatenated.
 *
 * @note The resulting concatenated string_view is stored as a static constexpr
 *       member `value`.
 */
template <const std::string_view &...args>
struct cxpr_stream {
   public:
    /// Total size of the concatenated string_view.
    constexpr static std::size_t size = (args.size() + ... + 0);

   private:
    /// Internal static array holding the concatenated characters.
    constexpr static std::array<char, size> arr = []<typename... Args>(Args &&...as) consteval {
        std::array<char, size> arr;
        std::size_t index = 0;
        (std::for_each(as.begin(), as.end(), [&](char c) { arr[index++] = c; }),
         ...);
        return arr;
    }(std::forward<const std::string_view &>(args)...);

   public:
    /// The concatenated string_view spanning the static array.
    constexpr static std::string_view value{arr.begin(), arr.end()};
};

}  // namespace circus::utils
