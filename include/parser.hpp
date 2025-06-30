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
        typedef std::variant<char, std::string, int, double> circ_primary_var_t;
        typedef std::unordered_map<std::string, circ_primary_var_t> circ_object_t;
        typedef std::variant<char, std::string, int, double, circ_object_t> circ_type_var_t;

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
            if (f_eof())
            {
                return f_peek();
            }
            return _in[_curs++];
        };

        bool f_eof() const
        {
            return traits::any_of(f_peek()._token_type, tokens__::TYPE::TK_EOF);
        };

        bool f_match(tokens__::TYPE type)
        {
            if (traits::any_of(f_peek()._token_type, std::forward<tokens__::TYPE>(type)))
            {
                f_advance();
                return true;
            }
            return false;
        }

        circ_variable *f_parse_assignment()
        {
            circ_variable *var = new circ_variable();
            if (f_match(tokens__::TYPE::TK_IDENTIFIER))
            {
                std::string key = f_advance()._embedded;
                var->key = key;
                if (f_match(tokens__::TYPE::TK_COLON))
                {
                }
                else
                {
                    // this is certainly an error
                };
            }
            else
            {
                // this is certainly an error
            };
        };

        circ_variable *f_parse_variable()
        {
            if (f_match(tokens__::TYPE::TK_DOLLA))
            {
                return f_parse_assignment();
            }
            return nullptr;
        }

        std::vector<circ_variable *> f_parse()
        {
            std::vector<circ_variable *> ret{};
            while (!f_eof())
            {
                ret.push_back(f_parse_variable()); // maybe emplace and move?!
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