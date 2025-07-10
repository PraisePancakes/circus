#pragma once
#include <filesystem>
#include <variant>

#include "circus_traits.hpp"
#include "token.hpp"
#include "utils/to_chars.hpp"

namespace circus {
#define CIRCUS_ENTRY(VAR) std::make_pair(#VAR, VAR)

template <typename OStreamT>
    requires(std::is_base_of_v<std::ostream, OStreamT>)
class serializer {
    OStreamT &stream;
    using TK = tokens__::TYPE;

    template <std::size_t I, std::size_t Max, traits::OutStreamableLiteral Arg>
    void handle_value_type(Arg &&arg) {
        if constexpr (traits::StringLike<Arg>) {
            stream << tokens__::to_literal(TK::TK_QUOTE_DOUBLE) << arg << tokens__::to_literal(TK::TK_QUOTE_DOUBLE);
        } else {
            stream << arg;
        }
        stream << tokens__::to_literal(TK::TK_SPACE);
    }

    template <std::size_t I, std::size_t Max, traits::Serializable Arg>
    void handle_value_type(Arg &&arg) {
        stream << tokens__::to_literal(TK::TK_CURL_L);
        arg.serialize(*this);
        stream << tokens__::to_literal(TK::TK_CURL_R);
    }

    template <std::size_t I, std::size_t Max, traits::StreamableVector Arg>
    void handle_value_type(Arg &&arg) {
        stream << tokens__::to_literal(TK::TK_BRACE_L);
        for (std::size_t i = 0; i < arg.size(); i++) {
            stream << arg[i];
            if (i < arg.size() - 1) {
                stream << tokens__::to_literal(TK::TK_COMMA);
            }
        };
        stream << tokens__::to_literal(TK::TK_BRACE_R);
    }

    template <std::size_t I, std::size_t Max, traits::Container Arg>
    void handle_value_type(Arg &&arg) {
        for (auto &a : arg) {
            handle_pair(a);
        }
    }

    template <std::size_t I, std::size_t Max, traits::PairSerializable Arg>
    void handle_pair(Arg &&arg) {
        stream << tokens__::to_literal(TK::TK_DOLLA) << arg.first << tokens__::to_literal(TK::TK_COLON);
        handle_value_type<I - 1, Max>(arg.second);
        if constexpr (I < Max - 1) {
            stream << tokens__::to_literal(TK::TK_COMMA);
        }
    }

    template <std::size_t I, typename Arg>
    constexpr static auto make_pair_serializable(Arg &&arg) {
        if constexpr (!traits::PairSerializable<Arg>) {
            return std::make_pair(circus::utils::num_to_string<I>::value, std::forward<Arg>(arg));
        } else {
            return std::forward<Arg>(arg);
        }
    }

    template <std::size_t... Is, typename... Args>
    void handler(std::index_sequence<Is...>, Args &&...args) {
        constexpr std::size_t MAXARG = sizeof...(Args);
        (handle_pair<Is, MAXARG>(make_pair_serializable<Is>(std::forward<Args>(args))), ...);
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