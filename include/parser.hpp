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
        constexpr static auto to_literal(std::variant<Args...> var)
        {
            return std::visit([](auto &&val) -> circ_variable::circ_type_var_t
                              { return std::forward<decltype(val)>(val); }, var);
        }

        circ_variable::circ_type_var_t f_parse_expression()
        {
            if (f_match(tokens__::TYPE::TK_CURL_L))
            {
                return f_parse();
            }
            if (f_match(tokens__::TYPE::TK_BRACE_L))
            {
                return f_parse_array();
            }
            f_advance();
            return to_literal(f_previous()._literal);
        };

        std::vector<circ_variable *> f_parse_array()
        {

            std::vector<circ_variable *> ret{};
            std::size_t index = 0;
            while (!f_eof() && !f_match(tokens__::TYPE::TK_BRACE_R))
            {

                auto type_basis = to_literal(f_advance()._literal);
                circ_variable *val = new circ_variable();

                val->key = tokens__::to_string(f_previous()._token_type) + std::to_string(index++);
                val->value = type_basis;

                ret.push_back(val);
            };
            std::cout << tokens__::to_string(f_peek()._token_type);

            return ret;
        };

        circ_variable *f_parse_decl()
        {
            using TK = tokens__::TYPE;
            if (f_match(TK::TK_DOLLA))
            {
                if (f_match(TK::TK_IDENTIFIER))
                {
                    circ_variable *var = new circ_variable();
                    var->key = f_previous()._embedded;
                    if (f_match(TK::TK_COLON))
                    {
                        std::cout << var->key << std::endl;
                        var->value = f_parse_expression();
                        if (!f_eof() && !f_match(TK::TK_COMMA))
                        {
                            if (!f_eof() && f_match(TK::TK_CURL_R))
                            {
                                while (f_match(TK::TK_CURL_R))
                                    ;
                            }
                            if (!f_eof() && !f_match(TK::TK_COMMA))
                            {
                                f_peek().print_token();
                                std::cerr << "MISSING COMMA" << std::endl;
                                throw std::runtime_error("MISSING COMMA");
                            }
                        }
                    }
                    else
                    {
                        std::cerr << "MISSING COLON" << std::endl;
                        throw std::runtime_error("MISSING COLON");
                    }
                }
            };
            return nullptr;
        }

        std::vector<circ_variable *> f_parse()
        {
            std::vector<circ_variable *> ret{};
            while (!f_eof())
            {

                ret.push_back(f_parse_decl()); // maybe emplace and move?!
            };
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