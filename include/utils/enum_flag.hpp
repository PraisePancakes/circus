#pragma once
#include <cstdint>
#include <iostream>

#include "../circus_traits.hpp"
namespace circus::utils {
template <typename T>
    requires(traits::Flaggable<T>)
class enum_flag {
    static_assert(traits::Flaggable<T>,
                  "T must be an enum with u32 or u64 underlying type");

   public:
    using enum_type = T;
    using underlying_type = typename std::underlying_type<T>::type;
    underlying_type flags{0x00u};

    enum_flag() = default;
    enum_flag(T initial) : flags(static_cast<underlying_type>(initial)) {};
    enum_flag &operator=(T flags) {
        *this |= flags;
        return *this;
    };

    constexpr enum_flag &operator|=(T fs) {
        flags |= static_cast<underlying_type>(fs);
        return *this;
    };

    constexpr enum_flag &operator&=(T fs) {
        flags &= static_cast<underlying_type>(fs);
        return *this;
    };

    template <typename U>
    [[nodiscard]] constexpr bool has(U e) const noexcept {
        static_assert((traits::Flaggable<U>), "Types must be flaggable");
        static_assert((std::is_same_v<U, enum_type>),
                      "Types must match 'this' enum type");
        return ((flags & static_cast<underlying_type>(e)) ==
                static_cast<underlying_type>(e));
    }

    template <typename... Args>
    [[nodiscard]] constexpr bool has_any(Args &&...args) const noexcept {
        return (has(std::forward<Args>(args)) || ...);
    }

    template <typename... Args>
    [[nodiscard]] constexpr bool has_all(Args &&...args) const noexcept {
        return (has(std::forward<Args>(args)) && ...);
    }

    constexpr bool operator==(const enum_flag<T> &other) const noexcept {
        return this->flags == other.flags;
    }

    constexpr bool operator!=(const enum_flag<T> &other) const noexcept {
        return this->flags != other.flags;
    }

    constexpr bool operator()() { return flags != 0; };
    ~enum_flag() = default;
};

template <typename E>
    requires(traits::Flaggable<E>)
E operator|(E l, E r) {
    static_assert(traits::Flaggable<E>, "type must be an enum and flaggable");
    using ut = typename std::underlying_type<E>::type;
    ut v = static_cast<ut>(l) | static_cast<ut>(r);
    return static_cast<E>(v);
}

template <typename E>
E operator&(E l, E r) {
    static_assert(traits::Flaggable<E>, "type must be an enum and flaggable");
    using ut = typename std::underlying_type<E>::type;
    ut v = static_cast<ut>(l) & static_cast<ut>(r);
    return static_cast<E>(v);
}

}  // namespace circus::utils