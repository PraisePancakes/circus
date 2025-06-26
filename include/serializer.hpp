#pragma once
#include <filesystem>
#include "token.hpp"
#include "circus_traits.hpp"
#include <variant>

namespace circus
{

    template <typename OStreamT>
        requires(std::is_base_of_v<std::ostream, OStreamT>)
    class serializer
    {
        OStreamT &stream;

        template <traits::OutStreamable T>
        void serialize_streamable(const T &value)
        {
            stream << value << tokens__::to_literal(tokens__::TYPE::TK_SPACE);
        }

        template <traits::Serializable T>
        void serialize_serializable(T &value)
        {
            value.serialize(*this);
        }

        template <traits::PairSerializable T>
        void serialize_pair(const T &value)
        {
            stream << tokens__::to_literal(tokens__::TYPE::TK_DOLLA) << value.first << tokens__::to_literal(tokens__::TYPE::TK_COLON);
            if constexpr (std::is_class_v<std::remove_cvref_t<decltype(value.second)>> && !traits::StreamableVector<decltype(value.second)>)
            {
                stream << tokens__::to_literal(tokens__::TYPE::TK_CURL_L);
                handle(value.second);
                stream << tokens__::to_literal(tokens__::TYPE::TK_CURL_R);
            }
            else
            {
                handle(value.second);
            }
        }

        template <traits::StreamableVector T>
        void serialize_vector(const T &vector)
        {
            stream << tokens__::to_literal(tokens__::TYPE::TK_BRACE_L);
            for (std::size_t i = 0; i < vector.size(); i++)
            {
                stream << vector[i];
                if (i < vector.size() - 1)
                {
                    stream << tokens__::to_literal(tokens__::TYPE::TK_COMMA);
                }
            };
            stream << tokens__::to_literal(tokens__::TYPE::TK_BRACE_R);
        }

        template <traits::OutStreamable Arg>
        void handle(Arg &&arg) { serialize_streamable(arg); }

        template <traits::Serializable Arg>
        void handle(Arg &&arg) { serialize_serializable(arg); }

        template <traits::PairSerializable Arg>
        void handle(Arg &&arg) { serialize_pair(arg); }

        template <traits::StreamableVector Arg>
        void handle(Arg &&arg) { serialize_vector(arg); }

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