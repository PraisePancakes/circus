#pragma once
#include <filesystem>
#include "token.hpp"
#include "circus_traits.hpp"
#include <variant>

namespace circus
{
#define CIRCUS_ENTRY(VAR) std::make_pair(#VAR, VAR)

    template <typename OStreamT>
        requires(std::is_base_of_v<std::ostream, OStreamT>)
    class serializer
    {
        OStreamT &stream;

        template <traits::OutStreamable Arg>
        void handle(Arg &&arg)
        {
            if constexpr (traits::StringLike<Arg>)
            {
                stream << tokens__::to_literal(tokens__::TYPE::TK_QUOTE_DOUBLE) << arg << tokens__::to_literal(tokens__::TYPE::TK_QUOTE_DOUBLE) << tokens__::to_literal(tokens__::TYPE::TK_SPACE);
            }
            else
            {
                stream << arg << tokens__::to_literal(tokens__::TYPE::TK_SPACE);
            }
        }

        template <traits::Serializable Arg>
        void handle(Arg &&arg) { arg.serialize(*this); }

        template <traits::PairSerializable Arg>
        void handle(Arg &&arg)
        {
            using value_type = decltype(arg.second);
            stream << tokens__::to_literal(tokens__::TYPE::TK_DOLLA) << arg.first << tokens__::to_literal(tokens__::TYPE::TK_COLON);
            if constexpr (std::is_class_v<std::remove_cvref_t<value_type>> && !traits::StreamableVector<value_type>)
            {
                stream << tokens__::to_literal(tokens__::TYPE::TK_CURL_L);
                handle(arg.second);
                stream << tokens__::to_literal(tokens__::TYPE::TK_CURL_R);
            }
            else
            {
                handle(arg.second);
            }
        }

        template <traits::StreamableVector Arg>
        void handle(Arg &&arg)
        {
            stream << tokens__::to_literal(tokens__::TYPE::TK_BRACE_L);
            for (std::size_t i = 0; i < arg.size(); i++)
            {
                stream << arg[i];
                if (i < arg.size() - 1)
                {
                    stream << tokens__::to_literal(tokens__::TYPE::TK_COMMA);
                }
            };
            stream << tokens__::to_literal(tokens__::TYPE::TK_BRACE_R);
        }

    public:
        serializer(OStreamT &s) : stream(s) {
                                  };

        template <typename... Args>
        void operator()(Args &&...args) &
        {
            (handle(std::forward<Args>(args)), ...);
        }

        template <typename... Args>
        serializer &operator<<(Args &&...args)
        {
            (handle(std::forward<Args>(args)), ...);
            return *this;
        }

        ~serializer() {};
    };
}