/**
 * @file enum_flag.hpp
 * @brief Provides a type-safe wrapper for enum flags with bitwise operations.
 */

#pragma once
#include <cstdint>
#include <iostream>

#include "../circus_traits.hpp"

namespace circus::utils {

/**
 * @brief A utility class for strongly typed enum flags with bitwise operations.
 *
 * @tparam T Enum type that meets the `traits::Flaggable` concept
 *           (must be enum with underlying type uint32_t or uint64_t).
 *
 * This class wraps an enum type and provides bitwise flag manipulation methods
 * and checks such as `has`, `has_any`, and `has_all`.
 */
template <typename T>
    requires(traits::Flaggable<T>)
class enum_flag {
    static_assert(traits::Flaggable<T>,
                  "T must be an enum with u32 or u64 underlying type");

   public:
    using enum_type = T;                                             ///< Underlying enum type.
    using underlying_type = typename std::underlying_type<T>::type;  ///< Enum underlying integer type.

    /// Stores the bit flags as underlying integer type.
    underlying_type flags{0x00u};

    /// Default constructor initializes with no flags set.
    enum_flag() = default;

    /**
     * @brief Constructs enum_flag with an initial flag value.
     * @param initial Enum flag to initialize with.
     */
    enum_flag(T initial) : flags(static_cast<underlying_type>(initial)) {};

    /**
     * @brief Assignment operator from enum type.
     * @param flags Enum flags to assign.
     * @return Reference to self.
     */
    enum_flag &operator=(T flags) {
        *this |= flags;
        return *this;
    };

    /**
     * @brief Bitwise OR assignment operator to add flags.
     * @param fs Flag to OR with current flags.
     * @return Reference to self.
     */
    constexpr enum_flag &operator|=(T fs) {
        flags |= static_cast<underlying_type>(fs);
        return *this;
    };

    /**
     * @brief Bitwise AND assignment operator to mask flags.
     * @param fs Flag mask to AND with current flags.
     * @return Reference to self.
     */
    constexpr enum_flag &operator&=(T fs) {
        flags &= static_cast<underlying_type>(fs);
        return *this;
    };

    /**
     * @brief Checks if this enum_flag contains all bits of a given flag.
     * @tparam U Enum type (must be same as T).
     * @param e Enum flag to check.
     * @return true if all bits are set, false otherwise.
     */
    template <typename U>
    [[nodiscard]] constexpr bool has(U e) const noexcept {
        static_assert((traits::Flaggable<U>), "Types must be flaggable");
        static_assert((std::is_same_v<U, enum_type>),
                      "Types must match 'this' enum type");
        return ((flags & static_cast<underlying_type>(e)) ==
                static_cast<underlying_type>(e));
    }

    /**
     * @brief Checks if this enum_flag contains any of the specified flags.
     * @tparam Args Variadic enum flags.
     * @param args Flags to check.
     * @return true if any flag is set, false otherwise.
     */
    template <typename... Args>
    [[nodiscard]] constexpr bool has_any(Args &&...args) const noexcept {
        return (has(std::forward<Args>(args)) || ...);
    }

    /**
     * @brief Checks if this enum_flag contains all of the specified flags.
     * @tparam Args Variadic enum flags.
     * @param args Flags to check.
     * @return true if all flags are set, false otherwise.
     */
    template <typename... Args>
    [[nodiscard]] constexpr bool has_all(Args &&...args) const noexcept {
        return (has(std::forward<Args>(args)) && ...);
    }

    /// Equality operator.
    constexpr bool operator==(const enum_flag<T> &other) const noexcept {
        return this->flags == other.flags;
    }

    /// Inequality operator.
    constexpr bool operator!=(const enum_flag<T> &other) const noexcept {
        return this->flags != other.flags;
    }

    /**
     * @brief Boolean conversion operator indicating if any flag is set.
     * @return true if any flag is set, false otherwise.
     */
    constexpr bool operator()() const noexcept { return flags != 0; };

    /// Destructor.
    ~enum_flag() = default;
};

}  // namespace circus::utils

/**
 * @brief Bitwise OR operator for enum types flagged by circus::traits::Flaggable.
 * @tparam E Enum type.
 * @param lhs Left operand.
 * @param rhs Right operand.
 * @return Bitwise OR of lhs and rhs.
 */
template <typename E>
    requires circus::traits::Flaggable<E>
constexpr E operator|(E lhs, E rhs) {
    using U = std::underlying_type_t<E>;
    return static_cast<E>(static_cast<U>(lhs) | static_cast<U>(rhs));
}

/**
 * @brief Bitwise AND operator for enum types flagged by circus::traits::Flaggable.
 * @tparam E Enum type.
 * @param lhs Left operand.
 * @param rhs Right operand.
 * @return Bitwise AND of lhs and rhs.
 */
template <typename E>
    requires circus::traits::Flaggable<E>
constexpr E operator&(E lhs, E rhs) {
    using U = std::underlying_type_t<E>;
    return static_cast<E>(static_cast<U>(lhs) & static_cast<U>(rhs));
}

/**
 * @brief Bitwise NOT operator for enum types flagged by circus::traits::Flaggable.
 * @tparam E Enum type.
 * @param val Operand.
 * @return Bitwise NOT of val.
 */
template <typename E>
    requires circus::traits::Flaggable<E>
constexpr E operator~(E val) {
    using U = std::underlying_type_t<E>;
    return static_cast<E>(~static_cast<U>(val));
}
