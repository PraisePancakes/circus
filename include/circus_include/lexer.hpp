/**
 * @file lexer.hpp
 * @brief Lexer class for tokenizing Circus input strings.
 */

#pragma once
#include <cassert>
#include <cctype>
#include <iostream>
#include <variant>
#include <vector>

#include "../../debug/debug.hpp"
#include "circus_traits.hpp"
#include "reader.hpp"
#include "token.hpp"
#include "visitor.hpp"

namespace circus {

/**
 * @class lexer__
 * @brief Lexer for tokenizing input strings into Circus tokens.
 *
 * This class processes an input string and produces a vector of tokens
 * representing literals, identifiers, reserved symbols, and comments.
 *
 * It maintains internal cursor position tracking for line and column.
 */
class lexer__ {
    std::string _in;              ///< The input string to lex.
    std::size_t _beg;             ///< Start index of current token.
    std::size_t _end;             ///< Current position in input.
    std::size_t _row;             ///< Current row (line) number.
    std::size_t _col;             ///< Current column number.
    std::vector<tokens__> _toks;  ///< Vector holding generated tokens.

    /**
     * @brief Convert a character to a token type enum.
     * @param c Character to convert.
     * @return Corresponding token type.
     */
    [[nodiscard]] static tokens__::TYPE f_token(char c) noexcept {
        return (tokens__::TYPE)c;
    }

    /**
     * @brief Check if a token type is a reserved symbol.
     * @param token_type Token type to check.
     * @return true if reserved symbol, false otherwise.
     */
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

    /**
     * @brief Debug helper to print all tokens.
     */
    void f_print() const noexcept {
        for (const auto &t : _toks) {
            t.print_token();
        }
    };

    /**
     * @brief Check if lexer has reached end of input.
     * @return true if end of input reached, false otherwise.
     */
    [[nodiscard]] bool f_eof() const noexcept {
        return (f_token(_in[_end]) == tokens__::TYPE::TK_EOF);
    }

    /**
     * @brief Peek the next character without consuming it.
     * @return The next character.
     */
    [[nodiscard]] char f_peek_next() const noexcept {
        return _in[_end + 1];
    }

    /**
     * @brief Peek the current character without consuming it.
     * @return The current character.
     */
    [[nodiscard]] char f_peek() const noexcept {
        return _in[_end];
    };

    /**
     * @brief Peek character at a given offset from current position.
     * @param offset Offset from current position.
     * @return The character at the offset.
     */
    [[nodiscard]] char f_peek_at(std::size_t offset) const noexcept {
        return _in[_end + offset];
    }

    /**
     * @brief Consume the current character and advance the cursor.
     * Updates row and column counters on newline.
     * @return The consumed character.
     */
    char f_advance() noexcept {
        if (f_eof())
            return '\0';

        if (_in[_end] == '\n') {
            _col = 0;
            _row++;
        } else {
            _col++;
        }

        return _in[_end++];
    };

    /**
     * @brief Get the previously consumed character.
     * @return The previous character.
     */
    char f_previous() const noexcept {
        return _in[_end - 1];
    }

    /**
     * @brief Get substring from current token start to current position.
     * @return Substring representing the current lexeme.
     */
    std::string to_substr() const noexcept {
        return _in.substr(_beg, _end - _beg);
    };

    /**
     * @brief Get substring with explicit start and length.
     * @param new_beg Start index.
     * @param new_end Length of substring.
     * @return Substring of input.
     */
    std::string to_substr(std::size_t new_beg, std::size_t new_end) const noexcept {
        return _in.substr(new_beg, new_end);
    };

    /**
     * @brief Scan and lex a numeric literal (int or float).
     */
    void scan_number() noexcept {
        while (!f_eof() && std::isdigit(f_peek()))
            f_advance();

        if (!f_eof() && f_peek() == '.') {
            f_advance();
            while (!f_eof() && std::isdigit(f_peek()))
                f_advance();
            insert(tokens__::TYPE::TK_LITERAL_FLOAT, to_substr(), std::stof(to_substr()));
        } else {
            insert(tokens__::TYPE::TK_LITERAL_INT, to_substr(), std::stoi(to_substr()));
        }
    };

    /**
     * @brief Scan and lex an identifier token.
     */
    void scan_identifier() noexcept {
        while (!f_eof() && std::isalnum(f_peek())) {
            f_advance();
        };
        insert(tokens__::TYPE::TK_IDENTIFIER, to_substr(), to_substr());
    };

    /**
     * @brief Scan a single reserved symbol token.
     */
    void scan_singular_reserve() noexcept {
        if (f_reserved(f_token(f_previous()))) {
            insert(f_token(f_previous()), to_substr(), f_previous());
        }
    };

    /**
     * @brief Scan a quoted string literal.
     */
    void scan_string() {
        while (!f_eof() && traits::none_of(f_token(f_advance()), tokens__::TYPE::TK_QUOTE_DOUBLE));

        insert(tokens__::TYPE::TK_LITERAL_STRING, to_substr(), to_substr());
    };

    /**
     * @brief Scan and skip comments (single line and block).
     */
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

    /**
     * @brief Handle unknown or invalid characters by inserting an unknown token.
     */
    void scan_unknown() noexcept {
        insert(tokens__::TYPE::TK_UNKNOWN, to_substr(), to_substr());
    };

    /**
     * @brief Create a token of specified type with embedded string and literal value.
     * @tparam T Literal type.
     * @param type Token type.
     * @param embedded The lexeme string.
     * @param literal Literal value associated with token.
     * @return Constructed token.
     */
    template <typename T>
    [[nodiscard]] constexpr tokens__ create_token(tokens__::TYPE type, std::string embedded, const T &literal) noexcept {
        tokens__ tok(type, embedded, literal, std::make_pair(_row, _col));
        return tok;
    }

    /**
     * @brief Insert a token into the token vector.
     * @tparam T Literal type.
     * @param type Token type.
     * @param embedded The lexeme string.
     * @param lit Literal value associated with token.
     */
    template <typename T>
    constexpr void insert(tokens__::TYPE type, std::string embedded, const T &lit) noexcept {
        _toks.push_back(create_token(type, embedded, lit));
    }

    /**
     * @brief Process one lexing unit (character or token) at current position.
     */
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

    /**
     * @brief Perform lexing on the input string.
     * @return Vector of tokens extracted from input.
     */
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
    /**
     * @brief Default constructor.
     * Initializes internal state.
     */
    lexer__() noexcept : _in{},
                         _beg{0},
                         _end{0},
                         _row{1},
                         _col{0},
                         _toks{} {};

    /**
     * @brief Lex an input string and return tokens.
     * @param input Input string to lex.
     * @return Vector of tokens.
     */
    [[nodiscard]] std::vector<tokens__> operator()(const std::string &input) noexcept {
        _in = input;
        return f_lex();
    };

    lexer__(lexer__ &&other) = default;
    lexer__(const lexer__ &other) = default;
    lexer__ &operator=(const lexer__ &other) = default;

    /**
     * @brief Get the input string currently being lexed.
     * @return Reference to input string.
     */
    const std::string &get_input() const { return _in; };

    /**
     * @brief Destructor.
     */
    ~lexer__() = default;
};

}  // namespace circus
