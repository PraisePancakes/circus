#pragma once
#include <filesystem>
#include "lexer.hpp"
#include "reader.hpp"
#include "parser.hpp"
#include "circus_traits.hpp"

namespace circus
{

    template <typename StreamType>
    class circ_format
    {
        StreamType &stream;

    public:
        template <typename Arg>
        void handle(Arg &&arg)
        {
            if constexpr (traits::Streamable<Arg>)
            {
                stream << arg << " ";
            }
            else if constexpr (traits::Serializable<Arg>)
            {
                arg.serialize(*this);
            }
            else if constexpr (traits::PairSerializable<Arg>)
            {
                stream << "$" << arg.first << " : ";
                if constexpr (std::is_class_v<decltype(arg.second)>)
                {
                    stream << "{";
                    handle(arg.second);
                    stream << "}";
                }
                else
                {
                    handle(arg.second);
                }
            }
            else
            {
                static_assert([]
                              { return false; }(), "Unsupported type");
            }
        }

        circ_format(StreamType &s) : stream(s) {};
    };

    template <typename StreamType, typename FormatPolicy = circ_format<StreamType>>
    class serializer
    {
        FormatPolicy formatter;

    public:
        serializer(StreamType &s) : formatter(s) {
                                    };

        template <typename... Args>
        void operator()(Args &&...args) &
        {
            (formatter.handle(std::forward<Args>(args)), ...);
        }

        template <typename... Args>
        serializer &operator<<(Args &&...args)
        {
            (formatter.handle(std::forward<Args>(args)), ...);
            return *this;
        }

        ~serializer() {};
    };
}