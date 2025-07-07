#pragma once
#include <iostream>
#include <stack>

#include "circus_traits.hpp"
#include "utils/enum_flag.hpp"

namespace circus::error {

template <typename E>
class error_reporter {
    using flag_type = circus::utils::enum_flag<E>;
    std::stack<std::string> error_log_stack;
    std::stack<std::string> temp;
    flag_type flagger;

   public:
    error_reporter() = default;

    [[nodiscard]] bool empty() const noexcept {
        return error_log_stack.empty() && !flagger();
    }

    template <typename... Es>
        requires(std::is_same_v<Es, flag_type::enum_type> && ...)
    void report(Es&&... es, const std::string& message) noexcept {
        flagger |= (es | ...);
        error_log_stack.push(message);
    }

    void log_errors() noexcept {
        while (!error_log_stack.empty()) {
            auto top = error_log_stack.top();
            std::clog << top << std::endl;
            temp.push(top);
            error_log_stack.pop();
        }

        while (!temp.empty()) {
            auto top = temp.top();
            error_log_stack.push(top);
            temp.pop();
        }
    }

    [[nodiscard]] const flag_type& get_flagger() const noexcept {
        return this->flagger;
    }

    void flush_errors() noexcept {
        while (!error_log_stack.empty()) {
            error_log_stack.pop();
        }
    };

    ~error_reporter() = default;
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