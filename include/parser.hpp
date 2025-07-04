#pragma once
#include <iostream>
#include <vector>
#include "token.hpp"
#include "circus_traits.hpp"
#include <unordered_map>
#include <any>

namespace circus
{
    struct circ_variable
    {
        typedef std::variant<char, std::string, int, float, double, std::vector<circ_variable *>> circ_type_var_t;

        std::string key;
        circ_type_var_t value;
    };
    class parser__
    {
        using TK = tokens__::TYPE;
        std::vector<tokens__> _in;
        std::size_t _curs;

        [[nodiscard]] tokens__ f_peek_at(const std::size_t off) const noexcept
        {
            return _in[_curs + off];
        };

        [[nodiscard]] tokens__ f_previous() const noexcept
        {
            return _in[_curs - 1];
        };

        [[nodiscard]] tokens__ f_peek_next() const noexcept
        {
            return _in[_curs + 1];
        };

        [[nodiscard]] tokens__ f_peek() const noexcept
        {
            return _in[_curs];
        };

        tokens__ f_advance() noexcept
        {
            if (!f_eof())
                _curs++;
            return f_previous();
        };

        bool f_eof() const
        {
            return traits::any_of(f_peek()._token_type, tokens__::TYPE::TK_EOF);
        };

        bool check(tokens__::TYPE t)
        {
            if (f_eof())
                return false;

            return f_peek()._token_type == t;
        }

        template <typename... Args>
            requires(std::is_same_v<Args, tokens__::TYPE> && ...)
        bool f_match(Args &&...args)
        {
            if ((check(args) || ...))
            {
                f_advance();
                return true;
            }
            return false;
        }

        template <typename... Args>
        circ_variable::circ_type_var_t to_value(std::variant<Args...> var)
        {
            return std::visit([](auto &&val) -> circ_variable::circ_type_var_t
                              { return std::forward<decltype(val)>(val); }, var);
        }

        circ_variable::circ_type_var_t f_parse_primary()
        {
            return to_value(f_advance().literal);
        }

        std::vector<circ_variable *> f_parse_array()
        {
            std::vector<circ_variable *> ret{};
            std::size_t index = 0;
            while (!f_eof() && f_peek()._token_type != TK::TK_BRACE_R)
            {
                circ_variable *var = new circ_variable();
                var->value = f_parse_primary();
                var->key = std::to_string(index++);

                if (!check(TK::TK_BRACE_R))
                {
                    if (!f_match(TK::TK_COMMA))
                    {
                        throw std::runtime_error("MISSING COMMA");
                    }
                    else if (check(TK::TK_BRACE_R))
                    {
                        throw std::runtime_error("TRAILING COMMA");
                    }
                }

                ret.push_back(var);
            }

            return ret;
        }

        circ_variable *f_parse_decl()
        {

            while (!f_eof() && f_match(TK::TK_DOLLA))
            {
                if (f_match(TK::TK_IDENTIFIER))
                {
                    circ_variable *var = new circ_variable();
                    var->key = f_previous().embedded;
                    std::cout << var->key << std::endl;
                    if (f_match(TK::TK_COLON))
                    {
                        if (f_match(TK::TK_CURL_L))
                        {
                            var->value = f_parse();
                            if (!f_match(TK::TK_CURL_R))
                            {
                                throw std::runtime_error("MISSING CLOSURE");
                            }
                        }
                        else if (f_match(TK::TK_BRACE_L))
                        {
                            var->value = f_parse_array();
                            if (!f_match(TK::TK_BRACE_R))
                            {
                                f_peek().print_token();
                                throw std::runtime_error("MISSING ARRAY CLOSURE");
                            }
                        }
                        else
                        {
                            var->value = f_parse_primary();
                        }

                        if (f_match(TK::TK_COMMA))
                        {
                            if (f_eof() || (f_match(TK::TK_CURL_R) || f_match(TK::TK_BRACE_R)))
                            {
                                throw std::runtime_error("UNEXPECTED COMMA");
                            }
                        }
                    }
                    else
                    {
                        throw std::runtime_error("MISSING COLON");
                    }
                    return var;
                }
                else
                {
                    throw std::runtime_error("MISSING IDENTIFIER");
                }
            };
            return nullptr;
        }

        std::vector<circ_variable *> f_parse()
        {
            std::vector<circ_variable *> ret{};

            while (!f_eof())
            {
                if (check(TK::TK_CURL_R))
                    break;

                if (circ_variable *decl = f_parse_decl())
                    ret.push_back(decl);
            }

            return ret;
        }

    public:
        parser__() : _in{}, _curs{0} {};
        std::vector<circ_variable *> operator()(std::vector<tokens__> toks)
        {
            _in = toks;
            return f_parse();
        };

        ~parser__() {};
    };

};