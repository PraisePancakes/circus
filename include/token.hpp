#pragma once
#include <iostream>
#include <variant>
#include "visitor.hpp"

namespace circus
{
    struct tokens__
    {

        // https://stackoverflow.com/questions/1801363/c-c-any-way-to-get-reflective-enums
#define TOKEN_DEFs                     \
    TOKEN_DEF(TK_QUOTE_DOUBLE, '"')    \
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
// ENUM DEFINITION
#define TOKEN_DEF(NAME, VALUE) NAME = VALUE,

        enum TYPE : unsigned char
        {
            TOKEN_DEFs
        };

#undef TOKEN_DEF

        using literal_variant_t = std::variant<char, std::string, int, float, double>;
        using location_t = std::pair<std::size_t, std::size_t>;
        TYPE _token_type;
        constexpr static inline const char *to_string(TYPE type) noexcept
        {
            switch (type)
            {
#define TOKEN_DEF(NAME, VALUE) \
    case NAME:                 \
        return #NAME;
                TOKEN_DEFs
#undef TOKEN_DEF
                    default : return "UNKNOWN_TYPE";
            }
        }

        constexpr static inline const char *to_stringized(TYPE type) noexcept
        {
            switch (type)
            {
#define TOKEN_DEF(NAME, VALUE) \
    case NAME:                 \
        return #VALUE;
                TOKEN_DEFs
#undef TOKEN_DEF
                    default : return "UNKNOWN_LITERAL";
            }
        }

        constexpr static inline std::string to_literal(TYPE type) noexcept
        {
            const char *lit = to_stringized(type);
            std::string ret{lit};
            return ret.substr(1, ret.size() - 2);
        }

    public:
        std::string embedded;
        literal_variant_t literal;
        location_t location;

        void print_token() const noexcept
        {
            std::cout << "TOKEN TYPE ID (" << to_literal(_token_type) << ")" << " [" << to_string(_token_type) << "]\n";
            std::cout << "location (row, col) < " <<  location.first << " , " <<  location.second << " > ";
            std::visit(internal::visitor{
                           [](char c)
                           { std::cout << "[CHAR] " << c << std::endl; },
                           [](std::string s)
                           {
                               std::cout << "[STRING] " << s << std::endl;
                           },
                           [](int i)
                           { std::cout << "[INT] " << i << std::endl; },
                           [](float f)
                           { std::cout << "[FLOAT] " << f << std::endl; },
                           [](double d)
                           { std::cout << "[DOUBLE] " << d << std::endl; },
                       },
                        literal);
        };

        tokens__(TYPE type, std::string embedded, literal_variant_t lit, location_t loc)
            : _token_type(type),  embedded(embedded),  literal(lit),  location(loc) {};
    };
};