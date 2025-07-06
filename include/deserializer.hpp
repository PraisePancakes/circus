#pragma once
#include <iostream>

#include "lexer.hpp"
#include "parser.hpp"

namespace circus {
template <typename IStreamT>
    requires(std::is_base_of_v<std::istream, IStreamT>)
class deserializer {
   public:
    deserializer(IStreamT &is) {
        std::string source = circus::filesystem::reader__{}(is);
        std::vector<circus::tokens__> tokens = circus::lexer__{}(std::move(source));
        std::vector<circus::circ_variable *> vars = circus::parser__{}(std::move(tokens));
    };

    ~deserializer() {};
};

}  // namespace circus