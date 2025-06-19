#pragma once
#include <iostream>

namespace circus::types
{
    template <typename Policy, typename Curr, typename... Others>
        requires(std::is_same<Curr, Policy>::value && (std::is_same<Others, Policy>::value && ...))
    static bool any_of(const Curr &c, const Others &&...o)
    {
        return ((c == o) || ...);
    }
};