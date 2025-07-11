#pragma once
#include <iostream>
#include <variant>

#include "visitor.hpp"

namespace circus {

/**
 * @struct tokens__
 * @brief Represents a lexical token with type, literal value, and source location.
 *
 * This struct defines the set of token types (using an enum), and stores
 * the literal value associated with the token, its embedded string form,
 * and its source location (row, column).
 */
struct tokens__ {
    // https://stackoverflow.com/questions/1801363/c-c-any-way-to-get-reflective-enums
    /// Token type definitions as enum values
#define TOKEN_DEFs                     \
    TOKEN_DEF(TK_QUOTE_DOUBLE, '\"')   \
    TOKEN_DEF(TK_PAREN_L, '(')         \
    TOKEN_DEF(TK_PAREN_R, ')')         \
    TOKEN_DEF(TK_COMMA, ',')           \
    TOKEN_DEF(TK_COLON, ':')           \
    TOKEN_DEF(TK_BRACE_L, '[')         \
    TOKEN_DEF(TK_BRACE_R, ']')         \
    TOKEN_DEF(TK_DOLLA, '$')           \
    TOKEN_DEF(TK_CURL_L, '{')          \
    TOKEN_DEF(TK_CURL_R, '}')          \
    TOKEN_DEF(TK_QUOTE_SINGLE, '\'')   \
    TOKEN_DEF(TK_STAR, '*')            \
    TOKEN_DEF(TK_SLASH, '/')           \
    TOKEN_DEF(TK_EOF, '\0')            \
    TOKEN_DEF(TK_SPACE, ' ')           \
    TOKEN_DEF(TK_NEWLINE, '\n')        \
    TOKEN_DEF(TK_LITERAL_INT, 0xFE)    \
    TOKEN_DEF(TK_LITERAL_FLOAT, 0xFD)  \
    TOKEN_DEF(TK_IDENTIFIER, 0xFC)     \
    TOKEN_DEF(TK_LITERAL_STRING, 0xFB) \
    TOKEN_DEF(TK_LITERAL_DOUBLE, 0xFA) \
    TOKEN_DEF(TK_UNKNOWN, 0xFF)

    /// Enum representing token types
#define TOKEN_DEF(NAME, VALUE) NAME = VALUE,
    enum TYPE : unsigned char {
        TOKEN_DEFs
    };
#undef TOKEN_DEF

    /// Variant type to hold possible literal values of tokens
    using literal_variant_t = std::variant<char, std::string, int, float, double>;

    /// Type representing the token location as (row, column)
    using location_t = std::pair<std::size_t, std::size_t>;

    /// The type of the token
    TYPE _token_type;

    /**
     * @brief Get string representation of token enum name.
     *
     * @param type Token type enum value
     * @return const char* Token name as string
     */
    constexpr static inline const char *to_string(TYPE type) noexcept {
        switch (type) {
#define TOKEN_DEF(NAME, VALUE) \
    case NAME:                 \
        return #NAME;
            TOKEN_DEFs
#undef TOKEN_DEF
                default : return "UNKNOWN_TYPE";
        }
    }

    /**
     * @brief Get string representation of token literal value.
     *
     * @param type Token type enum value
     * @return const char* Token literal as string
     */
    constexpr static inline const char *to_stringized(TYPE type) noexcept {
        switch (type) {
#define TOKEN_DEF(NAME, VALUE) \
    case NAME:                 \
        return #VALUE;
            TOKEN_DEFs
#undef TOKEN_DEF
                default : return "UNKNOWN_LITERAL";
        }
    }

    /**
     * @brief Convert the token literal to a std::string without surrounding quotes.
     *
     * @param type Token type enum value
     * @return std::string Token literal without quotes
     */
    constexpr static inline std::string to_literal(TYPE type) noexcept {
        const char *lit = to_stringized(type);
        std::string ret{lit};
        return ret.substr(1, ret.size() - 2);
    }

   public:
    /// The embedded string representing the token as read from the source
    std::string embedded;

    /// The literal value held by the token
    literal_variant_t literal;

    /// The location of the token in source as (row, column)
    location_t location;

    /**
     * @brief Prints the token details to standard output.
     */
    void print_token() const noexcept {
        std::cout << "TOKEN TYPE ID (" << to_literal(_token_type) << ")"
                  << " [" << to_string(_token_type) << "]\n";
        std::cout << "location (row, col) < " << location.first << " , " << location.second << " > ";
        std::visit(internal::visitor{
                       [](char c) { std::cout << "[CHAR] " << c << std::endl; },
                       [](std::string s) { std::cout << "[STRING] " << s << std::endl; },
                       [](int i) { std::cout << "[INT] " << i << std::endl; },
                       [](float f) { std::cout << "[FLOAT] " << f << std::endl; },
                       [](double d) { std::cout << "[DOUBLE] " << d << std::endl; },
                   },
                   literal);
    };

    /**
     * @brief Constructor to create a token instance.
     *
     * @param type Token type enum
     * @param embedded String representing the token
     * @param lit Literal value of the token
     * @param loc Source location as (row, column)
     */
    tokens__(TYPE type, std::string embedded, literal_variant_t lit, location_t loc)
        : _token_type(type), embedded(embedded), literal(lit), location(loc) {};
};

};  // namespace circus
