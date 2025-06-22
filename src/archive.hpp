#pragma once
#include <filesystem>
#include "lexer.hpp"
#include "reader.hpp"
#include "parser.hpp"
#include "circus_traits.hpp"
#include <variant>

namespace circus
{

    template <typename StreamType>
    class serializer
    {
        StreamType &stream;

        template <traits::Streamable T>
        void serialize_streamable(const T &value)
        {
            stream << value << " ";
        }

        template <traits::Serializable T>
        void serialize_serializable(T &value)
        {
            value.serialize(*this);
        }

        template <traits::PairSerializable T>
        void serialize_pair(const T &value)
        {
            stream << "$" << value.first << " : ";
            if constexpr (std::is_class_v<std::remove_cvref_t<decltype(value.second)>>)
            {
                stream << "{";
                handle(value.second);
                stream << "}";
            }
            else
            {
                handle(value.second);
            }
        }

        template <traits::StreamableVector T>
        void serialize_vector(const T &vector)
        {
            for (std::size_t i = 0; i < vector.size(); i++)
            {
                stream << vector[i] << ",";
            };
        }

        template <traits::Streamable Arg>
        void handle(Arg &&arg) { serialize_streamable(arg); }

        template <traits::Serializable Arg>
        void handle(Arg &&arg) { serialize_serializable(arg); }

        template <traits::PairSerializable Arg>
        void handle(Arg &&arg) { serialize_pair(arg); }

        template <traits::StreamableVector Arg>
        void handle(Arg &&arg) { serialize_vector(arg); }

    public:
        serializer(StreamType &s) : stream(s) {
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