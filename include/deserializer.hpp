#pragma once
#include <iostream>
#include "lexer.hpp"
#include "parser.hpp"

namespace circus
{
    template <typename IStreamT>
        requires(std::is_base_of_v<std::istream, IStreamT>)
    class deserializer
    {
        IStreamT &stream;
        std::variant</*var, obj, literal*/> _parseables;

    public:
        deserializer(IStreamT &is) : stream(is)
        {
            std::string source = circus::filesystem::reader__(is);
            std::vector<circus::tokens__> tokens = circus::lexer__{}(std::move(source));
            
        };

        ~deserializer() {};
    };

}