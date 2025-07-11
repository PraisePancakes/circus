#pragma once
#include <cstdint>
#include <iostream>
#include <vector>
/**
 * @file circus_traits.hpp
 * @brief Provides compile-time traits and C++20 concepts for type introspection used throughout the Circus library.
 *
 * * @defgroup Traits Concepts and Traits
 * @brief Core concepts and type traits used by Circus serialization framework.
 * @{
 */
namespace circus::traits {
/**
 * @brief Checks if a type Ts exists among a variadic list of types Us.
 */
template <typename Ts, typename... Us>
struct exists {
    constexpr static bool value = (std::is_same_v<Ts, Us> || ...);
};
/**
 * @brief Concept that checks if all types match a given type.
 */
template <typename TypeComparablePolicy, typename... Ts>
concept ComparableTypes = (std::is_same_v<TypeComparablePolicy, Ts> && ...);

/**
 * @brief Returns true if `c` matches any of the `o...`.
 */
template <typename Curr, typename... Others>
    requires(ComparableTypes<Curr, Others...>)
[[nodiscard]] constexpr static bool any_of(const Curr &c, const Others &&...o) noexcept {
    return ((c == o) || ...);
}
/**
 * @brief Returns true if `c` matches none of the `o...`.
 */
template <typename Curr, typename... Others>
    requires(ComparableTypes<Curr, Others...>)
[[nodiscard]] constexpr static bool none_of(const Curr &c, Others &&...o) noexcept {
    return !(any_of(c, std::forward<Others>(o)...));
};
/**
 * @brief Concept that checks whether a type has a `.serialize(std::ostream&)` method.
 */
template <typename T>
concept IsSerializable = requires(T t) {
    {
        t.serialize(std::declval<std::ostream &>())
    };
};
/**
 * @brief Concept that checks if a type can be used as an output stream.
 */
template <typename T>
concept OutStream = std::is_convertible_v<T, std::ostream &>;
/**
 * @brief Concept for values that can be streamed using `<<` to an `std::ostream`.
 */
template <typename T>
concept OutStreamableLiteral =
    requires(std::ostream &os, T value) {
        {
            os << value
        }
        -> OutStream;
    };
/**
 * @brief Concept that checks if a type can be used as an input stream.
 */
template <typename T>
concept InStream = std::is_convertible_v<T, std::istream &>;
/**
 * @brief Concept for values that can be extracted from an `std::istream` using `>>`.
 */
template <typename T>
concept InStreamable = requires(std::istream &is, T value) {
    {
        value >> is
    } -> InStream;
};
/**
 * @brief Trait to determine if a type is a pair-like structure (has .first and .second).
 */
template <typename T, typename = void>
struct pair_inspect : std::false_type {
};

template <typename T>
struct pair_inspect<T, std::void_t<
                           decltype(std::declval<T>().first),
                           decltype(std::declval<T>().second)>> : std::true_type {
    using first_type = decltype(std::declval<T>().first);
    using second_type = decltype(std::declval<T>().second);
};
/**
 * @brief Concept for types that behave like key-value pairs.
 */
template <typename T>
concept PairSerializable = pair_inspect<T>::value;
/**
 * @brief Concept for types that can be converted to a string-like view.
 */
template <class T>
concept StringLike = std::is_convertible_v<T, std::string_view>;
/**
 * @brief Concept for types that are serializable via either literal streaming or custom `serialize`.
 */
template <typename T>
concept Serializable = OutStreamableLiteral<T> || IsSerializable<T>;
/**
 * @brief Trait to determine if a type is a `std::vector`.
 */
template <typename T>
struct is_vector : std::false_type {
    using value_type = T;
};

template <typename T, typename A>
struct is_vector<std::vector<T, A>> : std::true_type {
    using value_type = T;
};
/**
 * @brief Concept for enum types that are eligible for flagging (i.e., bitmask ops).
 */
template <typename T>
concept Flaggable = std::is_enum_v<T> && (std::is_same_v<std::underlying_type_t<T>, std::uint64_t> || std::is_same_v<std::underlying_type_t<T>, std::uint32_t>);
/**
 * @brief Concept for `std::vector`-like containers whose elements can be streamed.
 */
template <typename V>
concept StreamableVector =
    is_vector<std::remove_cvref_t<V>>::value &&
    OutStreamableLiteral<typename std::remove_cvref_t<V>::value_type>;
/**
 * @brief Concept for containers that satisfy a wide set of STL-like requirements.
 * Includes regularity, iterator properties, size access, etc.
 */
template <class T>
concept Container = requires(T a, const T b) {
    requires std::regular<T>;
    requires std::swappable<T>;
    requires std::same_as<typename T::reference, typename T::value_type &>;
    requires std::same_as<typename T::const_reference, const typename T::value_type &>;
    requires std::forward_iterator<typename T::iterator>;
    requires std::forward_iterator<typename T::const_iterator>;
    requires std::signed_integral<typename T::difference_type>;
    requires std::same_as<typename T::difference_type, typename std::iterator_traits<typename T::iterator>::difference_type>;
    requires std::same_as<typename T::difference_type, typename std::iterator_traits<typename T::const_iterator>::difference_type>;
    { a.begin() } -> std::same_as<typename T::iterator>;
    { a.end() } -> std::same_as<typename T::iterator>;
    { b.begin() } -> std::same_as<typename T::const_iterator>;
    { b.end() } -> std::same_as<typename T::const_iterator>;
    { a.cbegin() } -> std::same_as<typename T::const_iterator>;
    { a.cend() } -> std::same_as<typename T::const_iterator>;
    { a.size() } -> std::same_as<typename T::size_type>;
    { a.max_size() } -> std::same_as<typename T::size_type>;
    { a.empty() } -> std::convertible_to<bool>;
};

};  // namespace circus::traits

/** @} */  // end of Traits group