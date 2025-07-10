#pragma once
#include <algorithm>
#include <iostream>

namespace circus::utils {
template <const std::string_view &...args>
struct cxpr_stream {
   public:
    constexpr static std::size_t size = (args.size() + ... + 0);

   private:
    constexpr static std::array<char, size> arr = []<typename... Args>(Args &&...as) consteval {
        std::array<char, size> arr;
        std::size_t index = 0;
        (std::for_each(as.begin(), as.end(), [&](char c) { arr[index++] = c; }),
         ...);
        return arr;
    }(std::forward<const std::string_view &>(args)...);

   public:
    constexpr static std::string_view value{arr.begin(), arr.end()};
};

};  // namespace circus::utils