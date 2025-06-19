#pragma once
#include <iostream>

namespace circus::types
{

    template <typename TypeComparablePolicy, typename... Ts>
    concept ComparableTypes = (std::is_same_v<TypeComparablePolicy, Ts> && ...);

    template <typename Policy, typename Curr, typename... Others>
        requires(ComparableTypes<Policy, Curr, Others...>)
    [[nodiscard]] constexpr static bool any_of(const Curr &c, const Others &&...o) noexcept
    {
        return ((c == o) || ...);
    }

    template <typename Policy, typename Curr, typename... Others>
        requires(ComparableTypes<Policy, Curr, Others...>)
    [[nodiscard]] constexpr static bool none_of(const Curr &c, Others &&...o) noexcept
    {
        return !(any_of<Policy>(c, std::forward<Others>(o)...));
    };
};