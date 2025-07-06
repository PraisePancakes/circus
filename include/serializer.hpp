#pragma once
#include <filesystem>
#include <variant>

#include "circus_traits.hpp"
#include "token.hpp"

namespace circus {
#define CIRCUS_ENTRY(VAR) std::make_pair(#VAR, VAR)

template <typename OStreamT>
    requires(std::is_base_of_v<std::ostream, OStreamT>)
class serializer {
    OStreamT &stream;

    template <std::size_t I, std::size_t Max, traits::OutStreamable Arg>
    void handle(Arg &&arg) {
        if constexpr (traits::StringLike<Arg>) {
            stream << tokens__::to_literal(tokens__::TYPE::TK_QUOTE_DOUBLE) << arg << tokens__::to_literal(tokens__::TYPE::TK_QUOTE_DOUBLE);
        } else {
            stream << arg;
        }
        stream << tokens__::to_literal(tokens__::TYPE::TK_SPACE);
    }

    template <std::size_t I, std::size_t Max, traits::Serializable Arg>
    void handle(Arg &&arg) {
        arg.serialize(*this);
    }

    template <std::size_t I, std::size_t Max, traits::PairSerializable Arg>
    void handle(Arg &&arg) {
        using value_type = decltype(arg.second);
        stream << std::endl;
        stream << tokens__::to_literal(tokens__::TYPE::TK_DOLLA) << arg.first << tokens__::to_literal(tokens__::TYPE::TK_COLON);
        if constexpr (std::is_class_v<std::remove_cvref_t<value_type>> && !traits::StreamableVector<value_type> && !traits::StringLike<value_type>) {
            stream << tokens__::to_literal(tokens__::TYPE::TK_CURL_L);
            handle<I - 1, Max>(arg.second);
            stream << tokens__::to_literal(tokens__::TYPE::TK_CURL_R);
        } else {
            handle<I - 1, Max>(arg.second);
        }
        if constexpr (I < Max - 1) {
            stream << tokens__::to_literal(tokens__::TYPE::TK_COMMA);
        }
    }

    template <std::size_t I, std::size_t Max, traits::StreamableVector Arg>
    void handle(Arg &&arg) {
        stream << tokens__::to_literal(tokens__::TYPE::TK_BRACE_L);
        for (std::size_t i = 0; i < arg.size(); i++) {
            stream << arg[i];
            if (i < arg.size() - 1) {
                stream << tokens__::to_literal(tokens__::TYPE::TK_COMMA);
            }
        };
        stream << tokens__::to_literal(tokens__::TYPE::TK_BRACE_R);
    }

    template <std::size_t I, typename Arg>
    static auto make_pair_serializable(Arg &&arg) {
        if constexpr (!traits::PairSerializable<Arg>) {
            return std::make_pair("entry" + std::to_string(I), std::forward<Arg>(arg));
        } else {
            return std::forward<Arg>(arg);
        }
    }
    template <std::size_t... Is, typename... Args>
    void handler(std::index_sequence<Is...>, Args &&...args) {
        constexpr std::size_t MAXARG = sizeof...(Args);
        (handle<Is, MAXARG>(make_pair_serializable<Is>(std::forward<Args>(args))), ...);
    }

   public:
    serializer(OStreamT &s) : stream(s) {

                              };

    template <typename... Args>
    void operator()(Args &&...args) & {
        handler(std::index_sequence_for<Args...>{}, std::forward<Args>(args)...);
    }

    template <typename... Args>
    serializer &operator<<(Args &&...args) {
        handler(std::index_sequence_for<Args...>{}, std::forward<Args>(args)...);
        return *this;
    }

    ~serializer() {};
};
}  // namespace circus