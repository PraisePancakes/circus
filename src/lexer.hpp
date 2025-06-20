#pragma once
#include <iostream>
#include <variant>
#include "reader.hpp"
#include <vector>
#include <cassert>
#include "visitor.hpp"
#include "circus_types.hpp"
#include "../debug/debug.hpp"
#include <cctype>

namespace circus
{

    struct tokens__
    {
        using literal_variant_t = std::variant<unsigned char, std::string, int, float>;
        using location_t = std::pair<std::size_t, std::size_t>;
        enum class TYPE : unsigned char
        {
            // RESERVED UNITS
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
            TK_STAR = '*',
            TK_SLASH = '/',
            TK_NEWLINE = '\n',

            TK_EOF = '\0',

            // NON RESERVED UNITS

            TK_LITERAL_INT = 0xFE,
            TK_LITERAL_FLOAT = 0xFD,
            TK_IDENTIFIER = 0xFC,
            TK_LITERAL_STRING = 0xFB,

            // unknown
            TK_UNKNOWN = 0xFF

        } _token_type;

    protected:
        literal_variant_t _literal;
        location_t _location;

    public:
#if CIRCUS_DEBUG_PEDANTIC__
        std::string type_to_string() const noexcept
        {
            switch (_token_type)
            {
            case tokens__::TYPE::TK_QUOTE_DOUBLE:
                return "[TK_QUOTE_DOUBLE]";
            case tokens__::TYPE::TK_PAREN_L:
                return "[TK_PAREN_L]";
            case tokens__::TYPE::TK_PAREN_R:
                return "[TK_PAREN_R]";
            case tokens__::TYPE::TK_COMMA:
                return "[TK_COMMA]";
            case tokens__::TYPE::TK_COLON:
                return "[TK_COLON]";
            case tokens__::TYPE::TK_BRACE_L:
                return "[TK_BRACE_L]";
            case tokens__::TYPE::TK_BRACE_R:
                return "[TK_BRACE_R]";
            case tokens__::TYPE::TK_DOLLA:
                return "[TK_DOLLA]";
            case tokens__::TYPE::TK_CURL_L:
                return "[TK_CURL_L]";
            case tokens__::TYPE::TK_CURL_R:
                return "[TK_CURL_R]";
            case tokens__::TYPE::TK_QUOTE_SINGLE:
                return "[TK_QUOTE_SINGLE]";
            case tokens__::TYPE::TK_STAR:
                return "[TK_STAR]";
            case tokens__::TYPE::TK_SLASH:
                return "[TK_SLASH]";
            case tokens__::TYPE::TK_NEWLINE:
                return "[TK_NEWLINE]";
            case tokens__::TYPE::TK_EOF:
                return "[TK_EOF]";
            case tokens__::TYPE::TK_LITERAL_STRING:
                return "[TK_LITERAL_STRING]";
            case tokens__::TYPE::TK_LITERAL_INT:
                return "[TK_LITERAL_INT]";
            case tokens__::TYPE::TK_LITERAL_FLOAT:
                return "[TK_LITERAL_FLOAT]";
            case tokens__::TYPE::TK_IDENTIFIER:
                return "[TK_IDENTIFIER]";
            default:
                return "[UNRECOGNIZED TOKEN]";
            };
        };

        void print_token() const noexcept
        {
            std::cout << "TOKEN TYPE ID (" << static_cast<int>(_token_type) << ")" << " [" << type_to_string() << "]\n";
            std::cout << "location (row, col) < " << _location.first << " , " << _location.second << " > ";
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
                       _literal);
        };
#endif

        tokens__(TYPE type, literal_variant_t lit, location_t loc)
            : _token_type(type), _literal(lit), _location(loc) {};
    };

    class lexer__
    {

        std::string _in;
        std::size_t _beg;
        std::size_t _end;
        std::size_t _row;
        std::size_t _col;
        std::vector<tokens__> _toks;

        [[nodiscard]] static tokens__::TYPE f_token(unsigned char c) noexcept
        {
            return (tokens__::TYPE)c;
        }

        [[nodiscard]] static bool f_reserved(tokens__::TYPE token_type) noexcept
        {
            switch (token_type)
            {
            case tokens__::TYPE::TK_PAREN_L:
            case tokens__::TYPE::TK_PAREN_R:
            case tokens__::TYPE::TK_COMMA:
            case tokens__::TYPE::TK_COLON:
            case tokens__::TYPE::TK_BRACE_L:
            case tokens__::TYPE::TK_BRACE_R:
            case tokens__::TYPE::TK_DOLLA:
            case tokens__::TYPE::TK_CURL_L:
            case tokens__::TYPE::TK_CURL_R:
            case tokens__::TYPE::TK_STAR:
            case tokens__::TYPE::TK_NEWLINE:
            case tokens__::TYPE::TK_SLASH:
            case tokens__::TYPE::TK_EOF:
            case tokens__::TYPE::TK_QUOTE_DOUBLE:
            case tokens__::TYPE::TK_QUOTE_SINGLE:
                return true;
            default:
                return false;
            }
        }
#if CIRCUS_DEBUG_PEDANTIC__
        void f_print() const noexcept
        {
            for (const auto &t : _toks)
            {
                t.print_token();
            };
        };
#endif

        [[nodiscard]] bool f_eof() const noexcept
        {
            return (f_token(_in[_end]) == tokens__::TYPE::TK_EOF);
        }
        [[nodiscard]] unsigned char f_peek_next() const noexcept
        {
            return _in[_end + 1];
        }

        [[nodiscard]] unsigned char f_peek() const noexcept
        {
            return _in[_end];
        };

        [[nodiscard]] unsigned char f_peek_at(std::size_t offset) const noexcept
        {
            return _in[_end + offset];
        }

        unsigned char f_advance() noexcept
        {
            if (f_eof())
                return '\0';

            // update cursor metadata here to avoid having to update it in any other scan method
            if (_in[_end] == '\n')
            {
                _col = 0;
                _row++;
            }
            else
            {
                _col++;
            }

            return _in[_end++];
        };

        unsigned char f_previous() const noexcept
        {
            return _in[_end - 1];
        }

        std::string to_substr() noexcept
        {
            return _in.substr(_beg, _end - _beg);
        };

        std::string to_substr(std::size_t new_beg, std::size_t new_end)
        {
            return _in.substr(new_beg, new_end);
        };

        void scan_number() noexcept
        {

            while (!f_eof() && std::isdigit(f_peek()))
                f_advance();
            if (f_advance() == '.')
            {
                while (!f_eof() && std::isdigit(f_peek()))
                    f_advance();
                insert(tokens__::TYPE::TK_LITERAL_FLOAT, std::stof(to_substr()));
            }
            else
            {
                insert(tokens__::TYPE::TK_LITERAL_INT, std::stoi(to_substr()));
            }
        };

        void scan_identifier() noexcept
        {
            while (!f_eof() && std::isalnum(f_peek()))
            {
                f_advance();
            };
            insert(tokens__::TYPE::TK_IDENTIFIER, to_substr());
        };

        void scan_singular_reserve() noexcept
        {
            if (f_reserved(f_token(f_previous())))
            {
                insert(f_token(f_previous()), f_previous());
            }
        };

        void scan_string()
        {
            while (!f_eof() && types::none_of(f_token(f_advance()), tokens__::TYPE::TK_QUOTE_DOUBLE))
                ;

            insert(tokens__::TYPE::TK_LITERAL_STRING, to_substr());
        };

        void scan_comments() noexcept
        {
            if (f_token(f_peek()) == tokens__::TYPE::TK_SLASH)
            {
                while (!f_eof() && f_token(f_peek()) != tokens__::TYPE::TK_NEWLINE)
                    f_advance();
            }

            if (f_token(f_advance()) == tokens__::TYPE::TK_STAR)
            {
                const auto curr = f_token(f_peek());
                while (!f_eof() && types::none_of(curr, tokens__::TYPE::TK_STAR, tokens__::TYPE::TK_SLASH))
                {
                    f_advance();
                }
            }
        };

        void scan_unknown() noexcept
        {
            insert(tokens__::TYPE::TK_UNKNOWN, to_substr());
        };

        template <typename T>
        [[nodiscard]] constexpr tokens__ create_token(tokens__::TYPE type, const T &literal) noexcept
        {
            tokens__ tok(type, literal, std::make_pair(_row, _col));
            return tok;
        }

        template <typename T>
        constexpr void insert(tokens__::TYPE type, const T &lit) noexcept
        {
            _toks.push_back(create_token(type, lit));
        }

        void process_unit(unsigned char c)
        {
            if (f_token(c) == tokens__::TYPE::TK_SLASH)
            {
                scan_comments();
            }
            if (f_reserved(f_token(c)))
            {
                if (types::none_of(f_token(c), tokens__::TYPE::TK_SLASH, tokens__::TYPE::TK_NEWLINE, tokens__::TYPE::TK_QUOTE_DOUBLE))
                    scan_singular_reserve();
                else if (types::any_of(f_token(c), tokens__::TYPE::TK_QUOTE_DOUBLE))
                    scan_string();
            }
            else if (std::isdigit(c))
            {
                scan_number();
            }
            else if (std::isalnum(c))
            {
                scan_identifier();
            }
            else
            {
                if (!std::isspace(c))
                    scan_unknown();
            };
        }

    public:
        lexer__() : _in{""},
                    _beg{0},
                    _end{0},
                    _row{1},
                    _col{0},
                    _toks{} {};

        [[nodiscard]] std::vector<tokens__> operator()(const std::filesystem::path &fp) noexcept
        {
            _in = circus::filesystem::reader__{}(fp);
            while (!f_eof())
            {
                unsigned char c = f_advance();
                process_unit(c);
                _beg = _end;
            };
            insert(tokens__::TYPE::TK_EOF, f_peek());

#if CIRCUS_DEBUG_PEDANTIC__
            f_print();
#endif

            return _toks;
        };

        lexer__(lexer__ &&other) = default;
        lexer__(const lexer__ &other) = default;
        lexer__ &operator=(const lexer__ &other) = default;
        const std::string &get_input() const { return _in; };
        ~lexer__() = default;
    };
};