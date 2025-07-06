#pragma once
#include <iostream>
#include <stack>

#include "circus_traits.hpp"

namespace circus::error {
template <typename T>
    requires(traits::Flaggable<T>)
class reporter {
    using underlying_t = typename std::underlying_type<T>::type;
    std::stack<std::string> error_message_stack;
    underlying_t flags{0x000};

   public:
    using type = T;
    reporter() = default;
    reporter(T single_flag) : flags(static_cast<underlying_t>(single_flag)) {};
    reporter(const reporter &o) : flags(o.flags) {};
    reporter &operator=(T single_flag) {
        flags = static_cast<underlying_t>(single_flag);
        return *this;
    }

    constexpr reporter &operator|=(T flag) {
        flags |= static_cast<underlying_t>(flag);
        return *this;
    };

    constexpr reporter &operator&=(T flag) {
        flags &= static_cast<underlying_t>(flag);
        return *this;
    }

    constexpr bool operator==(T flag) const noexcept {
        return ((this->flags & static_cast<underlying_t>(flag)) == static_cast<underlying_t>(flag));
    }

    explicit operator bool() const noexcept {
        return flags != 0;
    }

    void report(const std::string &s) noexcept {
        error_message_stack.push(s);
    };

    void print_error_stack() noexcept {
        while (!error_message_stack.empty()) {
            auto s = error_message_stack.top();
            std::cout << s << std::endl;
            error_message_stack.pop();
        }
    };

    ~reporter() = default;
};

};  // namespace circus::error

template <typename E>
    requires std::is_enum_v<E>
constexpr E operator|(E lhs, E rhs) {
    using U = std::underlying_type_t<E>;
    return static_cast<E>(static_cast<U>(lhs) | static_cast<U>(rhs));
}

template <typename E>
    requires std::is_enum_v<E>
constexpr E operator&(E lhs, E rhs) {
    using U = std::underlying_type_t<E>;
    return static_cast<E>(static_cast<U>(lhs) & static_cast<U>(rhs));
}

template <typename E>
    requires std::is_enum_v<E>
constexpr E operator~(E val) {
    using U = std::underlying_type_t<E>;
    return static_cast<E>(~static_cast<U>(val));
}