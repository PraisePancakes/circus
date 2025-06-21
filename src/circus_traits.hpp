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

    template <typename T>
    concept IsSerializable = requires(T t) {
        {
            t.serialize(std::declval<std::ostream &>())
        };
    };

    template <typename T>
    concept Stream = std::is_convertible_v<T, std::ostream &>;

    template <typename T>
    concept Streamable =
        requires(std::ostream &os, T value) {
            {
                os << value
            }
            -> Stream;
        };

    template <typename T>
    struct pair_inspect
    {
        using first = decltype(std::declval<T>().first);
        using second = decltype(std::declval<T>().second);
    };

    template <class T>
    concept StringLike = std::is_convertible_v<T, std::string_view>;

    template <typename T>
    concept Serializable = Streamable<T> || IsSerializable<T>;

    template <typename T>
    concept PairSerializable = StringLike<typename pair_inspect<T>::first> && (Serializable<typename pair_inspect<T>::second>);

};