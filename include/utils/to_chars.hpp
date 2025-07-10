#pragma once

namespace circus::utils {
namespace detail {
template <unsigned... digits>
struct to_chars {
    static const char value[];
};

template <unsigned... digits>
constexpr char to_chars<digits...>::value[] = {('0' + digits)..., 0};

template <unsigned rem, unsigned... digits>
struct explode : explode<rem / 10, rem % 10, digits...> {};

template <unsigned... digits>
struct explode<0, digits...> : to_chars<digits...> {};
}  // namespace detail
template <unsigned num>
struct num_to_string : detail::explode<num> {};
}  // namespace circus::utils