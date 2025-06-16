#pragma once
#include <iostream>
#include <variant>
#include "reader.hpp"
#include <vector>
#include <cassert>

namespace circus
{

    struct tokens__
    {
        enum class TYPE : unsigned char
        {
            // RESERVED ASCII TOKENS
            TK_QUOTE_DOUBLE = '\"',
            TK_PAREN_L = '(',
            TK_PAREN_R = ')',
            TK_COMMA = ',',
            TK_COLON = ':',
            TK_BRACE_L = '[',
            TK_BRACE_R = ']',
            TK_DOLLA = '$',
            TK_CURL_L = '{',
            TK_CURL_R = '}',
            TK_QUOTE_SINGLE = '\'',
            TK_EOF = '\0',

            // NON RESERVED -ASCII TOKENS
            TK_LITERAL_STRING = 0xFF,
            TK_LITERAL_INT = 0xFE,
            TK_LITERAL_FLOAT = 0xFD,
            TK_IDENTIFIER = 0xFC

        } _token_type;

        std::variant<unsigned char, std::string, int, float> _literal;
    };

    class lexer__
    {

        std::string _in;
        std::size_t _beg;
        std::size_t _end;
        std::size_t _row;
        std::size_t _col;
        std::vector<tokens__> _toks;

        static tokens__::TYPE f_token(unsigned char c)
        {
            return (tokens__::TYPE)c;
        }

        static bool f_reserved(tokens__::TYPE token_type)
        {

            switch (token_type)
            {
            case (tokens__::TYPE)tokens__::TYPE::TK_QUOTE_DOUBLE:
            case (tokens__::TYPE)tokens__::TYPE::TK_PAREN_L:
            case (tokens__::TYPE)tokens__::TYPE::TK_PAREN_R:
            case (tokens__::TYPE)tokens__::TYPE::TK_COMMA:
            case (tokens__::TYPE)tokens__::TYPE::TK_COLON:
            case (tokens__::TYPE)tokens__::TYPE::TK_BRACE_L:
            case (tokens__::TYPE)tokens__::TYPE::TK_BRACE_R:
            case (tokens__::TYPE)tokens__::TYPE::TK_DOLLA:
            case (tokens__::TYPE)tokens__::TYPE::TK_CURL_L:
            case (tokens__::TYPE)tokens__::TYPE::TK_CURL_R:
            case (tokens__::TYPE)tokens__::TYPE::TK_QUOTE_SINGLE:
            case (tokens__::TYPE)tokens__::TYPE::TK_EOF:
                return true;
            default:
                return false;
            };
        };

        bool f_eof() const
        {
            return (tokens__::TYPE)_in[_end] == tokens__::TYPE::TK_EOF;
        }
        char f_peek_next() const
        {
            return _in[_end + 1];
        }

        char f_peek() const
        {
            return _in[_end];
        };

        char f_advance()
        {
            return _in[_end++];
        };

    public:
        lexer__() : _in{""},
                    _beg{0},
                    _end{0},
                    _row{0},
                    _col{0} {};

        lexer__(const std::string &fp) : _in{circus::filesystem::reader__{}(fp)},
                                         _beg{0},
                                         _end{0},
                                         _row{0},
                                         _col{0} {};

        std::vector<tokens__> operator()(const std::string &fp) && noexcept
        {
            _in = circus::filesystem::reader__{}(fp);
            std::vector<tokens__> ret{};
            if (_in.size() == 0)
            {
                ret.push_back(tokens__{._token_type = tokens__::TYPE::TK_EOF, ._literal = '\0'});
                return ret;
            }

            while (!f_eof())
            {
                tokens__ current_token;
                char c = f_advance();
                tokens__::TYPE ttype = f_token(c);
                if (f_reserved(ttype))
                {
                    current_token._token_type = ttype;
                    current_token._literal = c;
                }
                else
                {
                    if (std::isalnum(c))
                    {
                    };
                };
            };

            return {};
        };

        std::vector<tokens__> lex() & noexcept
        {
            return {};
        };

        lexer__(lexer__ &&other) = default;
        lexer__(const lexer__ &other) = default;
        lexer__ &operator=(const lexer__ &other) = default;
        const std::string &get_input() const { return _in; };
        ~lexer__() = default;
    };
};