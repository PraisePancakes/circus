#pragma once
#include <cassert>
#include <cctype>
#include <iostream>
#include <variant>
#include <vector>

#include "../debug/debug.hpp"
#include "circus_traits.hpp"
#include "reader.hpp"
#include "token.hpp"
#include "visitor.hpp"

namespace circus {

class lexer__ {
    std::string _in;
    std::size_t _beg;
    std::size_t _end;
    std::size_t _row;
    std::size_t _col;
    std::vector<tokens__> _toks;

    [[nodiscard]] static tokens__::TYPE f_token(char c) noexcept {
        return (tokens__::TYPE)c;
    }

    [[nodiscard]] static bool f_reserved(tokens__::TYPE token_type) noexcept {
        switch (token_type) {
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
            case tokens__::TYPE::TK_SLASH:
            case tokens__::TYPE::TK_EOF:
            case tokens__::TYPE::TK_QUOTE_DOUBLE:
            case tokens__::TYPE::TK_QUOTE_SINGLE:
                return true;
            default:
                return false;
        }
    }

    void f_print() const noexcept {
        for (const auto &t : _toks) {
            t.print_token();
        };
    };

    [[nodiscard]] bool f_eof() const noexcept {
        return (f_token(_in[_end]) == tokens__::TYPE::TK_EOF);
    }
    [[nodiscard]] char f_peek_next() const noexcept {
        return _in[_end + 1];
    }

    [[nodiscard]] char f_peek() const noexcept {
        return _in[_end];
    };

    [[nodiscard]] char f_peek_at(std::size_t offset) const noexcept {
        return _in[_end + offset];
    }

    char f_advance() noexcept {
        if (f_eof())
            return '\0';

        // update cursor metadata here to avoid having to update it in any other scan method
        if (_in[_end] == '\n') {
            _col = 0;
            _row++;
        } else {
            _col++;
        }

        return _in[_end++];
    };

    char f_previous() const noexcept {
        return _in[_end - 1];
    }

    std::string to_substr() const noexcept {
        return _in.substr(_beg, _end - _beg);
    };

    std::string to_substr(std::size_t new_beg, std::size_t new_end) const noexcept {
        return _in.substr(new_beg, new_end);
    };

    void scan_number() noexcept {
        while (!f_eof() && std::isdigit(f_peek()))
            f_advance();

        if (!f_eof() && f_peek() == '.') {
            f_advance();  // now consume the dot
            while (!f_eof() && std::isdigit(f_peek()))
                f_advance();
            insert(tokens__::TYPE::TK_LITERAL_FLOAT, to_substr(), std::stof(to_substr()));
        } else {
            insert(tokens__::TYPE::TK_LITERAL_INT, to_substr(), std::stoi(to_substr()));
        }
    };

    void scan_identifier() noexcept {
        while (!f_eof() && std::isalnum(f_peek())) {
            f_advance();
        };
        insert(tokens__::TYPE::TK_IDENTIFIER, to_substr(), to_substr());
    };

    void scan_singular_reserve() noexcept {
        if (f_reserved(f_token(f_previous()))) {
            insert(f_token(f_previous()), to_substr(), f_previous());
        }
    };

    void scan_string() {
        while (!f_eof() && traits::none_of(f_token(f_advance()), tokens__::TYPE::TK_QUOTE_DOUBLE));

        insert(tokens__::TYPE::TK_LITERAL_STRING, to_substr(), to_substr());
    };

    void scan_comments() noexcept {
        if (f_token(f_peek()) == tokens__::TYPE::TK_SLASH) {
            while (!f_eof() && f_token(f_peek()) != tokens__::TYPE::TK_NEWLINE)
                f_advance();
        }

        if (f_token(f_advance()) == tokens__::TYPE::TK_STAR) {
            const auto curr = f_token(f_peek());
            while (!f_eof() && traits::none_of(curr, tokens__::TYPE::TK_STAR, tokens__::TYPE::TK_SLASH)) {
                f_advance();
            }
        }
    };

    void scan_unknown() noexcept {
        insert(tokens__::TYPE::TK_UNKNOWN, to_substr(), to_substr());
    };

    template <typename T>
    [[nodiscard]] constexpr tokens__ create_token(tokens__::TYPE type, std::string embedded, const T &literal) noexcept {
        tokens__ tok(type, embedded, literal, std::make_pair(_row, _col));
        return tok;
    }

    template <typename T>
    constexpr void insert(tokens__::TYPE type, std::string embedded, const T &lit) noexcept {
        _toks.push_back(create_token(type, embedded, lit));
    }

    void process_unit() {
        char c = f_advance();
        if (f_token(c) == tokens__::TYPE::TK_SLASH) {
            scan_comments();
        }
        if (f_reserved(f_token(c))) {
            if (traits::none_of(f_token(c), tokens__::TYPE::TK_SLASH, tokens__::TYPE::TK_QUOTE_DOUBLE))
                scan_singular_reserve();
            else if (traits::any_of(f_token(c), tokens__::TYPE::TK_QUOTE_DOUBLE))
                scan_string();
        } else if (std::isdigit(c)) {
            scan_number();
        } else if (std::isalnum(c)) {
            scan_identifier();
        } else {
            if (!std::isspace(c))
                scan_unknown();
        };
    }

    [[nodiscard]] std::vector<tokens__> f_lex() & noexcept {
        while (!f_eof()) {
            process_unit();
            _beg = _end;
        };
        insert(tokens__::TYPE::TK_EOF, to_substr(), f_peek());

#if CIRCUS_DEBUG_PEDANTIC__

        f_print();
#endif

        return _toks;
    };

   public:
    lexer__() noexcept : _in{},
                         _beg{0},
                         _end{0},
                         _row{1},
                         _col{0},
                         _toks{} {};

    [[nodiscard]] std::vector<tokens__> operator()(const std::string &input) noexcept {
        _in = input;
        return f_lex();
    };

    lexer__(lexer__ &&other) = default;
    lexer__(const lexer__ &other) = default;
    lexer__ &operator=(const lexer__ &other) = default;
    const std::string &get_input() const { return _in; };
    ~lexer__() = default;
};
};  // namespace circus