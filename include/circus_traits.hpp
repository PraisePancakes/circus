#pragma once
#include <cstdint>
#include <iostream>
namespace circus::traits {
template <typename Ts, typename... Us>
struct exists {
    constexpr static bool value = (std::is_same_v<Ts, Us> || ...);
};

template <typename TypeComparablePolicy, typename... Ts>
concept ComparableTypes = (std::is_same_v<TypeComparablePolicy, Ts> && ...);

template <typename Curr, typename... Others>
    requires(ComparableTypes<Curr, Others...>)
[[nodiscard]] constexpr static bool any_of(const Curr &c, const Others &&...o) noexcept {
    return ((c == o) || ...);
}

template <typename Curr, typename... Others>
    requires(ComparableTypes<Curr, Others...>)
[[nodiscard]] constexpr static bool none_of(const Curr &c, Others &&...o) noexcept {
    return !(any_of(c, std::forward<Others>(o)...));
};

template <typename T>
concept IsSerializable = requires(T t) {
    {
        t.serialize(std::declval<std::ostream &>())
    };
};

template <typename T>
concept OutStream = std::is_convertible_v<T, std::ostream &>;

template <typename T>
concept OutStreamable =
    requires(std::ostream &os, T value) {
        {
            os << value
        }
        -> OutStream;
    };

template <typename T>
concept InStream = std::is_convertible_v<T, std::istream &>;

template <typename T>
concept InStreamable = requires(std::istream &is, T value) {
    {
        value >> is
    } -> InStream;
};

template <typename T, typename = void>
struct pair_inspect : std::false_type {
};

template <typename T>
struct pair_inspect<T, std::void_t<
                           decltype(std::declval<T>().first),
                           decltype(std::declval<T>().second)>> : std::true_type {
    using first_type = decltype(std::declval<T>().first);
    using second_type = decltype(std::declval<T>().second);
};

template <typename T>
concept PairSerializable = pair_inspect<T>::value;

template <class T>
concept StringLike = std::is_convertible_v<T, std::string_view>;

template <typename T>
concept Serializable = OutStreamable<T> || IsSerializable<T>;

template <typename T>
struct is_vector : std::false_type {
    using value_type = T;
};

template <typename T, typename A>
struct is_vector<std::vector<T, A>> : std::true_type {
    using value_type = T;
};

template <typename T>
concept Flaggable = std::is_enum_v<T> && (std::is_same_v<std::underlying_type_t<T>, std::uint64_t> || std::is_same_v<std::underlying_type_t<T>, std::uint32_t>);

template <typename V>
concept StreamableVector =
    is_vector<std::remove_cvref_t<V>>::value &&
    OutStreamable<typename std::remove_cvref_t<V>::value_type>;

};  // namespace circus::traits