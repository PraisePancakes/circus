#pragma once
#include <iostream>
#include <variant>
#include "reader.hpp"
#include <vector>
#include <cassert>
#include "visitor.hpp"

namespace circus
{

    struct tokens__
    {
        enum class TYPE : unsigned char
        {
            // RESERVED ASCII UNITS
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

            // NON RESERVED ASCII UNITS
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
            case (tokens__::TYPE)tokens__::TYPE::TK_PAREN_L:
            case (tokens__::TYPE)tokens__::TYPE::TK_PAREN_R:
            case (tokens__::TYPE)tokens__::TYPE::TK_COMMA:
            case (tokens__::TYPE)tokens__::TYPE::TK_COLON:
            case (tokens__::TYPE)tokens__::TYPE::TK_BRACE_L:
            case (tokens__::TYPE)tokens__::TYPE::TK_BRACE_R:
            case (tokens__::TYPE)tokens__::TYPE::TK_DOLLA:
            case (tokens__::TYPE)tokens__::TYPE::TK_CURL_L:
            case (tokens__::TYPE)tokens__::TYPE::TK_CURL_R:
            case (tokens__::TYPE)tokens__::TYPE::TK_EOF:
                return true;
            default:
                return false;
            };
        };

        void print_token() const
        {

            for (const auto &t : _toks)
            {
                switch (t._token_type)
                {
                case tokens__::TYPE::TK_IDENTIFIER:
                    std::cout << "[IDENTIFIER]" << std::endl;
                    break;
                case tokens__::TYPE::TK_LITERAL_INT:
                case tokens__::TYPE::TK_LITERAL_FLOAT:
                    std::cout << "[NUMBER]" << std::endl;
                    break;
                case tokens__::TYPE::TK_LITERAL_STRING:
                    std::cout << "[LITERAL STRING]" << std::endl;
                    break;
                default:
                    std::cout << "[RESERVED]" << std::endl;
                    break;
                };
                std::cout << "TypeID: " << static_cast<int>(t._token_type) << ", Literal: ";
                std::visit(internal::visitor{[](unsigned char c)
                                             { std::cout << "[UCHAR] " << c << std::endl; },
                                             [](std::string s)
                                             {
                                                 std::cout << "[STRING] " << s << std::endl;
                                             },
                                             [](int i)
                                             { std::cout << "[INT] " << i << std::endl; },
                                             [](float f)
                                             { std::cout << "[FLOAT] " << f << std::endl; }},
                           t._literal);
            };
        };

        bool f_eof() const
        {
            return (f_token(_in[_end]) == tokens__::TYPE::TK_EOF);
        }
        unsigned char f_peek_next() const
        {
            return _in[_end + 1];
        }

        unsigned char f_peek() const
        {
            return _in[_end];
        };

        unsigned char f_advance()
        {
            return _in[_end++];
        };

        void scan_number()
        {
            while (!f_eof() && std::isdigit(f_peek()))
            {
                f_advance();
            };
            _toks.push_back(create_token(tokens__::TYPE::TK_LITERAL_INT, std::stoi(_in.substr(_beg, _end - _beg))));
        };

        void scan_identifier()
        {
            while (!f_eof() && std::isalnum(f_peek()))
            {
                f_advance();
            };
            _toks.push_back(create_token(tokens__::TYPE::TK_IDENTIFIER, _in.substr(_beg, _end - _beg)));
        };

        void scan_string() {};

        template <typename T>
        tokens__ create_token(tokens__::TYPE type, const T &literal)
        {
            tokens__ tok{._token_type = type, ._literal = literal};
            return tok;
        }


    public:
        lexer__() : _in{""},
                    _beg{0},
                    _end{0},
                    _row{0},
                    _col{0} {};


        std::vector<tokens__> operator()(const std::string &fp) noexcept
        {
            _in = circus::filesystem::reader__{}(fp);
            if (_in.size() == 0)
            {
                _toks.push_back(tokens__{._token_type = tokens__::TYPE::TK_EOF, ._literal = '\0'});
                return _toks;
            }

            while (!f_eof())
            {
                unsigned char c = f_advance();
                if (f_reserved(f_token(c)))
                {
                    tokens__ tok = create_token(f_token(c), c);
                    _toks.push_back(tok);
                }
                else if (std::isdigit(c))
                {
                    scan_number();
                }
                else if (std::isalnum(c))
                {
                    scan_identifier();
                };

                _beg = _end;
            };

            _toks.push_back(create_token(tokens__::TYPE::TK_EOF, f_peek()));
            print_token();
            return _toks;
        };

        lexer__(lexer__ &&other) = default;
        lexer__(const lexer__ &other) = default;
        lexer__ &operator=(const lexer__ &other) = default;
        const std::string &get_input() const { return _in; };
        ~lexer__() = default;
    };
};