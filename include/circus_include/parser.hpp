#pragma once
#include <any>
#include <iostream>
#include <unordered_map>
#include <vector>

#include "circus_error.hpp"
#include "circus_traits.hpp"
#include "token.hpp"

namespace circus {

/**
 * @brief Represents a variable in Circus serialization.
 *
 * Supports primitive types, arrays, and nested objects.
 */
struct circ_variable {
    /// Variant holding possible Circus variable types.
    typedef std::variant<
        char,
        std::string,
        int,
        float,
        double,
        std::vector<circ_variable>,
        std::unordered_map<std::string, circ_variable>>
        circ_type_var_t;

    /// The key name associated with this variable.
    std::string key;

    /// The value stored in this variable.
    circ_type_var_t value;

    /**
     * @brief Access nested variables by key, for objects.
     *
     * @param key The key to access inside the object.
     * @return Reference to nested circ_variable.
     *
     * @note This assumes the current variable holds an object.
     */
    circ_variable& operator[](const std::string& key) {
        auto& m = std::get<std::unordered_map<std::string, circ_variable>>(value);
        return m[key];
    };
};

/**
 * @class parser__
 * @brief Parser for Circus token streams producing nested Circus variables.
 *
 * Parses tokens produced by the lexer into nested Circus objects and arrays,
 * handling syntax errors and reporting them.
 */
class parser__ {
    using TK = tokens__::TYPE;
    using error_type = error::parser_error::enum_type;

    /// Input tokens to parse
    std::vector<tokens__> _in;

    /// Current cursor position in the token stream
    std::size_t _curs;

    /// Parser error reporter instance
    circus::error::parser_reporter _reporter;

    /**
     * @brief Peek at token offset from current cursor.
     * @param off Offset from current cursor.
     * @return Token at position (_curs + off).
     */
    [[nodiscard]] tokens__ f_peek_at(const std::size_t off) const noexcept {
        return _in[_curs + off];
    };

    /**
     * @brief Get the previous token (before current cursor).
     * @return Token before the current cursor.
     */
    [[nodiscard]] tokens__ f_previous() const noexcept {
        return _in[_curs - 1];
    };

    /**
     * @brief Peek the next token after current cursor.
     * @return Token at (_curs + 1).
     */
    [[nodiscard]] tokens__ f_peek_next() const noexcept {
        return _in[_curs + 1];
    };

    /**
     * @brief Peek the current token at cursor.
     * @return Token at current cursor.
     */
    [[nodiscard]] tokens__ f_peek() const noexcept {
        return _in[_curs];
    };

    /**
     * @brief Advance cursor and return the previous token.
     * @return Token at previous cursor position before advance.
     */
    tokens__ f_advance() noexcept {
        if (!f_eof())
            _curs++;
        return f_previous();
    };

    /**
     * @brief Check if cursor reached end of tokens.
     * @return True if end of tokens reached.
     */
    bool f_eof() const {
        return traits::any_of(f_peek()._token_type, tokens__::TYPE::TK_EOF);
    };

    /**
     * @brief Check if current token type matches `t`.
     * @param t Token type to check.
     * @return True if current token matches type `t`.
     */
    bool check(tokens__::TYPE t) {
        if (f_eof())
            return false;
        return f_peek()._token_type == t;
    }

    /**
     * @brief Match and consume one of the specified token types.
     *
     * @tparam Args Token types to match.
     * @param args Token types passed as arguments.
     * @return True if any token type matched and was consumed.
     */
    template <typename... Args>
        requires(std::is_same_v<Args, tokens__::TYPE> && ...)
    bool f_match(Args&&... args) {
        if ((check(args) || ...)) {
            f_advance();
            return true;
        }
        return false;
    }

    /**
     * @brief Converts a variant token literal to the internal circ_type_var_t.
     *
     * @tparam Args Variant types.
     * @param var The variant to convert.
     * @return Converted circ_type_var_t holding the value.
     */
    template <typename... Args>
    [[nodiscard]] decltype(auto) to_value(std::variant<Args...> var) const noexcept {
        return std::visit([](const auto& val) -> circ_variable::circ_type_var_t { return std::forward<decltype(val)>(val); }, var);
    }

    /**
     * @brief Parse a primary expression: object, array, or literal.
     * @return Parsed circ_type_var_t representing the value.
     * @throws parser_error on syntax errors.
     */
    circ_variable::circ_type_var_t f_parse_primary() {
        if (f_match(TK::TK_CURL_L)) {
            std::unordered_map<std::string, circ_variable> obj = f_parse();
            if (f_match(TK::TK_CURL_R)) {
                return obj;
            }
            throw circus::error::parser_error(error_type::SYNTAX, "Missing Closing Curl '}' ");
        }

        if (f_match(TK::TK_BRACE_L)) {
            std::vector<circ_variable> arr = f_parse_array();
            if (!f_match(TK::TK_BRACE_R)) {
                throw circus::error::parser_error(error_type::SYNTAX, "Missing Closing Bracket ']' ");
            }
            return arr;
        }

        return to_value(f_advance().literal);
    }

    /**
     * @brief Parse an array of circ_variables.
     * @return Vector of parsed circ_variables.
     * @throws parser_error on syntax errors.
     */
    std::vector<circ_variable> f_parse_array() {
        std::vector<circ_variable> ret{};
        std::size_t index = 0;
        while (!f_eof() && !check(TK::TK_BRACE_R)) {
            circ_variable var = circ_variable();
            var.value = f_parse_primary();
            var.key = std::to_string(index++);
            if (!check(TK::TK_BRACE_R)) {
                if (!f_match(TK::TK_COMMA)) {
                    throw circus::error::parser_error(error_type::SYNTAX, "Missing Comma Separator ");
                }
            }
            ret.push_back(var);
        }
        if (check(TK::TK_BRACE_R) && f_previous()._token_type == TK::TK_COMMA) {
            throw circus::error::parser_error(error_type::SYNTAX, "Trailing Comma Separator ");
        }
        return ret;
    }

    /**
     * @brief Parse a value with a preceding key.
     * @return Parsed circ_variable with key and value.
     * @throws parser_error on syntax errors.
     */
    circ_variable f_parse_value() {
        circ_variable var = circ_variable();
        var.key = f_previous().embedded;
        if (f_match(TK::TK_COLON)) {
            var.value = f_parse_primary();
            if (!f_eof() && !check(TK::TK_CURL_R)) {
                if (!f_match(TK::TK_COMMA)) {
                    throw circus::error::parser_error(error_type::SYNTAX, "Missing Comma Separator ");
                }
            }
            return var;
        }
        throw circus::error::parser_error(error_type::SYNTAX, "Missing Colon Operator ");
    };

    /**
     * @brief Parse an identifier token and its associated value.
     * @return Parsed circ_variable.
     * @throws parser_error on syntax errors.
     */
    circ_variable f_parse_identifier() {
        if (f_match(TK::TK_IDENTIFIER)) {
            return f_parse_value();
        }
        throw circus::error::parser_error(error_type::SYNTAX, "Missing Identifier ");
    };

    /**
     * @brief Parse a declaration starting with '$' token.
     * @return Parsed circ_variable declaration.
     * @throws parser_error on syntax errors.
     */
    circ_variable f_parse_decl() {
        if (!f_eof() && f_match(TK::TK_DOLLA)) {
            return f_parse_identifier();
        };
        throw circus::error::parser_error(error_type::SYNTAX, "Missing Declaration Specifier '$' ");
    }

    /**
     * @brief Synchronize parser after an error to a stable state.
     */
    void f_sync() {
        if (!f_eof()) f_advance();
        while (!f_eof()) {
            if (check(TK::TK_DOLLA)) return;
            f_advance();
        }
    };

    /**
     * @brief Main parse function to parse object from tokens.
     * @return Parsed unordered_map<string, circ_variable> object.
     * @throws std::runtime_error if unrecoverable errors occurred.
     */
    std::unordered_map<std::string, circ_variable> f_parse() {
        std::unordered_map<std::string, circ_variable> ret{};
        bool had_error = false;
        while (!f_eof()) {
            try {
                if (check(TK::TK_CURL_R))
                    break;
                auto v = f_parse_decl();
                ret.insert(std::make_pair(v.key, v));
            } catch (circus::error::parser_error& error) {
                _reporter.report(error.type_of, error.what(), f_previous().location);
                f_sync();
                had_error = true;
            }
        }
        _reporter.log_errors();
        if (had_error) {
            throw std::runtime_error("halted execution due to parser incomprehension, revise circ source input");
        }
        return ret;
    }

   public:
    /**
     * @brief Default constructor.
     */
    parser__() : _in{}, _curs{0}, _reporter{} {};

    /**
     * @brief Parse tokens into Circus variables.
     * @param toks Vector of tokens from lexer.
     * @return Parsed root object as unordered_map.
     */
    std::unordered_map<std::string, circ_variable> operator()(std::vector<tokens__> toks) {
        _in = toks;
        return f_parse();
    };

    /**
     * @brief Destructor.
     */
    ~parser__() {};
};

}  // namespace circus
