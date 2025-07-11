#pragma once
#include <iostream>
#include <stack>
#include <unordered_map>

#include "circus_traits.hpp"
#include "utils/enum_flag.hpp"
/**
 * @file circus_error.hpp
 * @brief Defines error handling utilities for the Circus parser, including error types, exceptions, and reporting.
 */

namespace circus::error {

/**
 * @defgroup ParserErrors Parser Error Types and Handling
 * @brief Components related to parser error reporting and exception handling.
 * @{
 */

/**
 * @brief Enumeration of Circus parser error types.
 */
#define ERROR_TYPE(NAME) NAME
#define ERROR_TYPEs \
    ERROR_TYPE(SYNTAX)

enum CIRCUS_ERROR_TYPES : std::uint64_t {
    ERROR_TYPEs
};
#undef ERROR_TYPE
/**
 * @brief String names corresponding to error types.
 */
#define ERROR_TYPE(NAME) #NAME
inline const char* REFLECTED_ERROR_MAP[]{
    ERROR_TYPEs};
#undef ERROR_TYPE

/**
 * @class parser_error
 * @brief Exception class for parser errors, holding an error type flag and a message.
 */
class parser_error : public std::runtime_error {
   public:
    using flag_type = circus::utils::enum_flag<CIRCUS_ERROR_TYPES>;
    using enum_type = flag_type::enum_type;
    /// The type of parser error that occurred.
    flag_type type_of;
    /**
     * @brief Constructs a parser error with a flag type and descriptive message.
     * @param E The error flag.
     * @param what The error message.
     */
    parser_error(flag_type E, const std::string& what) : std::runtime_error(what), type_of(E) {};
};
/**
 * @class parser_reporter
 * @brief Aggregates and manages parser error messages with support for logging and flushing.
 */
class parser_reporter {
    using flag_type = circus::utils::enum_flag<CIRCUS_ERROR_TYPES>;
    using underlying_type = std::underlying_type_t<flag_type::enum_type>;
    std::stack<std::string> error_log_stack;
    std::stack<std::string> temp;

    /**
     * @brief Checks if a given error code matches an error type and returns its string representation.
     * @param from The error code.
     * @param to The error type to check against.
     * @return The string name of the matched error or empty string.
     */
    static std::string match_and_ret(const underlying_type& from, const underlying_type& to) {
        return ((from & to) == to) ? REFLECTED_ERROR_MAP[from] : "";
    };
    /**
     * @brief Converts error flags to a string representation.
     * @param type The error flags.
     * @return A string describing the error type or "UNKNOWN" if none matches.
     */
    static std::string to_stringized_types(const underlying_type& type) {
        std::string ret = "";
        ret += match_and_ret(type, error::SYNTAX);
        return ret.empty() ? "UNKNOWN" : ret;
    }

   public:
    parser_reporter() = default;

    /**
     * @brief Checks if there are any errors logged.
     * @return true if error stack is empty, false otherwise.
     */
    [[nodiscard]] bool empty() const noexcept {
        return error_log_stack.empty();
    }
    /**
     * @brief Reports an error by adding a formatted message to the error log stack.
     * @param type The error flag.
     * @param message The error message.
     * @param location The source location as (line, column).
     */
    void report(const flag_type& type, const std::string& message, std::pair<std::size_t, std::size_t> location) noexcept {
        std::string out = "[CIRCUS][PARSER_ERROR][" + to_stringized_types(type.flags) + "] " + message + " at (" + std::to_string(location.first) + " : " + std::to_string(location.second) + ")";
        error_log_stack.push(out);
    }
    /**
     * @brief Logs all errors to std::clog and preserves the error stack.
     */
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

    /**
     * @brief Clears all errors from the error log stack.
     */
    void flush_errors() noexcept {
        while (!error_log_stack.empty()) {
            error_log_stack.pop();
        }
    };

    ~parser_reporter() = default;
};
/**
 * @brief Bitwise OR operator for enum error types.
 * @tparam E Enum type.
 * @param lhs Left-hand operand.
 * @param rhs Right-hand operand.
 * @return Bitwise OR of lhs and rhs.
 */
template <typename E>
    requires std::is_enum_v<E>
constexpr E operator|(E lhs, E rhs) {
    using U = std::underlying_type_t<E>;
    return static_cast<E>(static_cast<U>(lhs) | static_cast<U>(rhs));
}
/**
 * @brief Bitwise AND operator for enum error types.
 * @tparam E Enum type.
 * @param lhs Left-hand operand.
 * @param rhs Right-hand operand.
 * @return Bitwise AND of lhs and rhs.
 */
template <typename E>
    requires std::is_enum_v<E>
constexpr E operator&(E lhs, E rhs) {
    using U = std::underlying_type_t<E>;
    return static_cast<E>(static_cast<U>(lhs) & static_cast<U>(rhs));
}
/**
 * @brief Bitwise NOT operator for enum error types.
 * @tparam E Enum type.
 * @param val Operand.
 * @return Bitwise NOT of val.
 */
template <typename E>
    requires std::is_enum_v<E>
constexpr E operator~(E val) {
    using U = std::underlying_type_t<E>;
    return static_cast<E>(~static_cast<U>(val));
}
};  // namespace circus::error

/** @} */  // end of ParserErrors group
