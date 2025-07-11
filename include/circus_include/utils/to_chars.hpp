/**
 * @file to_chars.hpp
 * @brief Compile-time conversion of unsigned integers to string literals.
 */

#pragma once
#include <iostream>

namespace circus::utils {

namespace detail {

/**
 * @brief Helper template that stores a compile-time char array of digits.
 * 
 * @tparam digits Pack of digit values (0-9).
 */
template <unsigned... digits>
struct to_chars {
    /// Null-terminated character array representing the number.
    static const char value[];
};

/** 
 * @brief Definition of the static char array containing digits plus terminating null.
 */
template <unsigned... digits>
constexpr char to_chars<digits...>::value[] = {('0' + digits)..., 0};

/**
 * @brief Compile-time recursion to split an unsigned integer into digits.
 * 
 * @tparam rem Remaining number to split.
 * @tparam digits Pack of digits collected so far.
 */
template <unsigned rem, unsigned... digits>
struct explode : explode<rem / 10, rem % 10, digits...> {};

/**
 * @brief Base case for explode recursion when remainder is zero.
 * Inherits from to_chars to produce the final digit string.
 * 
 * @tparam digits Pack of digits forming the number.
 */
template <unsigned... digits>
struct explode<0, digits...> : to_chars<digits...> {};

}  // namespace detail

/**
 * @brief Converts a compile-time unsigned integer to a string literal.
 * 
 * Usage: `num_to_string<123>::value` evaluates to `"123"`.
 * 
 * @tparam num Number to convert.
 */
template <unsigned num>
struct num_to_string : detail::explode<num> {};

/**
 * @brief Compile-time string equality check.
 * 
 * @param a Pointer to a null-terminated C-string.
 * @param b Pointer to a null-terminated C-string.
 * @return true if strings are equal, false otherwise.
 */
constexpr bool strings_equal(char const* a, char const* b) {
    return std::string_view(a) == b;
}

}  // namespace circus::utils
