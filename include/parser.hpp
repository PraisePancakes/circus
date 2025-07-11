#pragma once
#include <any>
#include <iostream>
#include <unordered_map>
#include <vector>

#include "circus_error.hpp"
#include "circus_traits.hpp"
#include "token.hpp"

namespace circus {
struct circ_variable {
    typedef std::variant<char, std::string, int, float, double, std::vector<circ_variable>, std::unordered_map<std::string, circ_variable>> circ_type_var_t;
    std::string key;
    circ_type_var_t value;
    // recursively find key from root variable
    circ_variable& operator[](const std::string& key) {
        auto& m = std::get<std::unordered_map<std::string, circ_variable>>(value);
        return m[key];
    };
};
class parser__ {
    using TK = tokens__::TYPE;
    using error_type = error::parser_error::enum_type;

    std::vector<tokens__> _in;
    std::size_t _curs;
    circus::error::parser_reporter _reporter;

    [[nodiscard]] tokens__ f_peek_at(const std::size_t off) const noexcept {
        return _in[_curs + off];
    };

    [[nodiscard]] tokens__ f_previous() const noexcept {
        return _in[_curs - 1];
    };

    [[nodiscard]] tokens__ f_peek_next() const noexcept {
        return _in[_curs + 1];
    };

    [[nodiscard]] tokens__ f_peek() const noexcept {
        return _in[_curs];
    };

    tokens__ f_advance() noexcept {
        if (!f_eof())
            _curs++;
        return f_previous();
    };

    bool f_eof() const {
        return traits::any_of(f_peek()._token_type, tokens__::TYPE::TK_EOF);
    };

    bool check(tokens__::TYPE t) {
        if (f_eof())
            return false;

        return f_peek()._token_type == t;
    }

    template <typename... Args>
        requires(std::is_same_v<Args, tokens__::TYPE> && ...)
    bool f_match(Args&&... args) {
        if ((check(args) || ...)) {
            f_advance();
            return true;
        }
        return false;
    }

    template <typename... Args>
    [[nodiscard]] decltype(auto) to_value(std::variant<Args...> var) const noexcept {
        return std::visit([](const auto& val) -> circ_variable::circ_type_var_t { return std::forward<decltype(val)>(val); }, var);
    }

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

    std::vector<circ_variable> f_parse_array() {
        std::vector<circ_variable> ret{};
        std::size_t index = 0;
        while (!f_eof() && !check(TK::TK_BRACE_R)) {
            circ_variable var = circ_variable();
            var.value = f_parse_primary();
            var.key = std::to_string(index++);
            if (!check(TK::TK_BRACE_R)) {
                if (!f_match(TK::TK_COMMA)) {
                    throw circus::error::parser_error(error_type::SYNTAX, "Missing Comma Seperator ");
                }
            }
            ret.push_back(var);
        }
        if (check(TK::TK_BRACE_R) && f_previous()._token_type == TK::TK_COMMA) {
            throw circus::error::parser_error(error_type::SYNTAX, "Trailing Comma Seperator ");
        }
        return ret;
    }

    circ_variable f_parse_value() {
        circ_variable var = circ_variable();
        var.key = f_previous().embedded;
        if (f_match(TK::TK_COLON)) {
            var.value = f_parse_primary();
            if (!f_eof() && !check(TK::TK_CURL_R)) {
                if (!f_match(TK::TK_COMMA)) {
                    throw circus::error::parser_error(error_type::SYNTAX, "Missing Comma Seperator ");
                }
            }
            return var;
        }
        throw circus::error::parser_error(error_type::SYNTAX, "Missing Colon Operator ");
    };

    circ_variable f_parse_identifier() {
        if (f_match(TK::TK_IDENTIFIER)) {
            return f_parse_value();
        }
        throw circus::error::parser_error(error_type::SYNTAX, "Missing Identifier ");
    };

    circ_variable f_parse_decl() {
        if (!f_eof() && f_match(TK::TK_DOLLA)) {
            return f_parse_identifier();
        };
        throw circus::error::parser_error(error_type::SYNTAX, "Missing Declaration Specifier '$' ");
    }

    void f_sync() {
        if (!f_eof()) f_advance();
        while (!f_eof()) {
            if (check(TK::TK_DOLLA)) return;
            f_advance();
        }
    };

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
    parser__() : _in{}, _curs{0}, _reporter{} {};
    std::unordered_map<std::string, circ_variable> operator()(std::vector<tokens__> toks) {
        _in = toks;
        return f_parse();
    };

    ~parser__() {};
};

};  // namespace circus