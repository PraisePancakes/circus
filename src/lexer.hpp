#pragma once
#include <iostream>
#include <variant>
#include "reader.hpp"

namespace circus
{
    struct tokens__
    {
        enum class TYPE : unsigned
        {
            TK_LCURL = '{',
            TK_RCURL = '}',
            TK_LPAREN = '(',
            TK_RPAREN = ')',
            TK_COMMA = ',',
            TK_COLON = ':',
            TK_QUOTE = '\"',
            TK_SINGLE_QUOTE = '\'',
            TK_EOF = '\0'
        };
    };

    template <typename... fs>
    struct visitor : fs...
    {
        using fs::operator()...;
    };

    class lexer__
    {
        std::variant<std::string, tokens__::TYPE> _current_lexeme;
        std::string _in;
        std::size_t _beg;
        std::size_t _end;
        std::size_t _row;
        std::size_t _col;

    public:
        lexer__() : _current_lexeme{},
                    _in{""},
                    _beg{0},
                    _end{0},
                    _row{0},
                    _col{0} {};

        lexer__(const std::string &fp) : _current_lexeme{}, _in{circus::filesystem::reader__{}(fp)}, _beg{0}, _end{0}, _row{0}, _col{0} {};
        lexer__(lexer__ &&other) = default;
        lexer__(const lexer__ &other) = default;
        lexer__ &operator=(const lexer__ &other) = default;
        const std::string &get_input() const { return _in; };
        ~lexer__() = default;
    };
};