#pragma once
#include <any>
#include <iostream>
#include <unordered_map>
#include <vector>

#include "circus_traits.hpp"
#include "token.hpp"

namespace circus {
struct circ_variable {
    typedef std::variant<char, std::string, int, float, double, std::vector<circ_variable *>> circ_type_var_t;

    std::string key;
    circ_type_var_t value;
};
class parser__ {
    using TK = tokens__::TYPE;
    std::vector<tokens__> _in;
    std::size_t _curs;

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
    bool f_match(Args &&...args) {
        if ((check(args) || ...)) {
            f_advance();
            return true;
        }
        return false;
    }

    template <typename... Args>
    [[nodiscard]] decltype(auto) to_value(std::variant<Args...> var) const noexcept {
        return std::visit([](const auto &val) -> circ_variable::circ_type_var_t { return std::forward<decltype(val)>(val); }, var);
    }

    circ_variable::circ_type_var_t f_parse_primary() {
        if (f_match(TK::TK_CURL_L)) {
            std::vector<circ_variable *> obj = f_parse();
            if (f_match(TK::TK_CURL_R)) {
                return obj;
            }
            throw std::runtime_error("MISSING CLOSING '}' ");
        }

        if (f_match(TK::TK_BRACE_L)) {
            std::vector<circ_variable *> arr = f_parse_array();
            if (!f_match(TK::TK_BRACE_R)) {
                throw std::runtime_error("MISSING ']' ");
            }
            return arr;
        }

        return to_value(f_advance().literal);
    }

    std::vector<circ_variable *> f_parse_array() {
        std::vector<circ_variable *> ret{};
        std::size_t index = 0;
        while (!f_eof() && !check(TK::TK_BRACE_R)) {
            circ_variable *var = new circ_variable();
            var->value = f_parse_primary();
            var->key = std::to_string(index++);
            if (!check(TK::TK_BRACE_R)) {
                if (!f_match(TK::TK_COMMA)) {
                    throw std::runtime_error("MISSING COMMA");
                }
            }
            ret.push_back(var);
        }
        if (check(TK::TK_BRACE_R) && f_previous()._token_type == TK::TK_COMMA) {
            throw std::runtime_error("TRAILING COMMA");
        }
        return ret;
    }

    circ_variable *f_parse_decl() {
        if (!f_eof() && f_match(TK::TK_DOLLA)) {
            if (f_match(TK::TK_IDENTIFIER)) {
                circ_variable *var = new circ_variable();
                var->key = f_previous().embedded;
                if (f_match(TK::TK_COLON)) {
                    var->value = f_parse_primary();
                    if (!f_eof() && !check(TK::TK_CURL_R)) {
                        if (!f_match(TK::TK_COMMA)) {
                            throw std::runtime_error("MISSING COMMA");
                        }
                    }
                    return var;
                }
                throw std::runtime_error("MISSING COLON");
            }
            throw std::runtime_error("MISSING IDENTIFIER");
        };
        throw std::runtime_error("MISSING '$");
        return nullptr;
    }

    std::vector<circ_variable *> f_parse() {
        std::vector<circ_variable *> ret{};

        while (!f_eof()) {
            if (check(TK::TK_CURL_R))
                break;
            ret.push_back(f_parse_decl());
        }

        return ret;
    }

   public:
    parser__() : _in{}, _curs{0} {};
    std::vector<circ_variable *> operator()(std::vector<tokens__> toks) {
        _in = toks;
        return f_parse();
    };

    ~parser__() {};
};

};  // namespace circus