#pragma once
#include <iostream>
#include <vector>
#include "token.hpp"
#include "circus_traits.hpp"

namespace circus
{
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
            _in[_curs - 1];
        };

        [[nodiscard]] tokens__ f_peek_next() const noexcept
        {
            return _in[_curs + 1];
        };

        [[nodiscard]] tokens__ f_peek() const noexcept
        {
            return _in[_curs];
        };

        [[nodiscard]] tokens__ f_advance() noexcept
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

    public:
        parser__() : _in{}, _curs{0} {};

        ~parser__() {};
    };

};