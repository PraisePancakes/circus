#pragma once
#include <iostream>

namespace circus::traits
{

    template <typename TypeComparablePolicy, typename... Ts>
    concept ComparableTypes = (std::is_same_v<TypeComparablePolicy, Ts> && ...);

    template <typename Curr, typename... Others>
        requires(ComparableTypes<Curr, Others...>)
    [[nodiscard]] constexpr static bool any_of(const Curr &c, const Others &&...o) noexcept
    {
        return ((c == o) || ...);
    }

    template <typename Curr, typename... Others>
        requires(ComparableTypes<Curr, Others...>)
    [[nodiscard]] constexpr static bool none_of(const Curr &c, Others &&...o) noexcept
    {
        return !(any_of(c, std::forward<Others>(o)...));
    };

};