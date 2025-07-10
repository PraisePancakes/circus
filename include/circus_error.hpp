#pragma once
#include <iostream>
#include <stack>
#include <unordered_map>

#include "circus_traits.hpp"
#include "utils/enum_flag.hpp"

namespace circus::error {
#define ERROR_TYPE(NAME) NAME
#define ERROR_TYPEs \
    ERROR_TYPE(SYNTAX)

enum CIRCUS_ERROR_TYPES : std::uint64_t {
    ERROR_TYPEs
};
#undef ERROR_TYPE
#define ERROR_TYPE(NAME) #NAME
const char* names_of[]{
    ERROR_TYPEs};
#undef ERROR_TYPE

class parser_error : public std::runtime_error {
   public:
    using flag_type = circus::utils::enum_flag<CIRCUS_ERROR_TYPES>;
    using enum_type = flag_type::enum_type;
    flag_type type_of;
    parser_error(flag_type E, const std::string& what) : std::runtime_error(what), type_of(E) {};
};

class parser_reporter {
    using flag_type = circus::utils::enum_flag<CIRCUS_ERROR_TYPES>;
    std::stack<std::string> error_log_stack;
    std::stack<std::string> temp;
    static std::string to_stringized_types(const std::underlying_type_t<flag_type::enum_type>& type) {
        std::string ret = "";
        if ((type & flag_type::enum_type::SYNTAX) == flag_type::enum_type::SYNTAX) ret += "SYNTAX";
        return ret.empty() ? "UNKNOWN" : ret;
    }

   public:
    parser_reporter() = default;

    [[nodiscard]] bool empty() const noexcept {
        return error_log_stack.empty();
    }

    void report(const flag_type& type, const std::string& message, std::pair<std::size_t, std::size_t> location) noexcept {
        std::string out = "[CIRCUS][PARSER_ERROR] " + message + to_stringized_types(type.flags) + " " + std::to_string(location.first) + " : " + std::to_string(location.second);
        error_log_stack.push(out);
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

    void flush_errors() noexcept {
        while (!error_log_stack.empty()) {
            error_log_stack.pop();
        }
    };

    ~parser_reporter() = default;
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