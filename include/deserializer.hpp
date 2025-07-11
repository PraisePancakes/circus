#pragma once
#include <iostream>

#include "lexer.hpp"
#include "parser.hpp"

namespace circus {

template <typename IStreamT>
    requires(std::is_base_of_v<std::istream, IStreamT>)
class deserializer {
    std::unordered_map<std::string, circ_variable> root;

    class circ_safe_proxy {
        circ_variable& var;

       public:
        circ_safe_proxy(circ_variable& v) : var(v) {};

        circ_safe_proxy operator[](const std::string& key) {
            return circ_safe_proxy(var[key]);
        };

        // disable ct warning on ambiguous operator[] with std::string
        circ_safe_proxy operator[](const char* key) {
            return circ_safe_proxy(var[std::string(key)]);
        }

        template <typename T>
        operator T() {
            return std::get<T>(var.value);
        }
    };

   public:
    using circ_object = std::unordered_map<std::string, circ_variable>;
    deserializer(IStreamT& is) {
        std::string source = circus::filesystem::reader__{}(is);
        std::vector<circus::tokens__> tokens = circus::lexer__{}(std::move(source));
        std::unordered_map<std::string, circ_variable> rt = circus::parser__{}(std::move(tokens));
        root = std::move(rt);
        // this will be a root obj in which you can recursively search through via [] operators instead of a vector of circ_variable*
        // circus::circ_variable var = parser__{}(std::move(tokens));
    };

    circ_safe_proxy operator[](const std::string& k) {
        return circ_safe_proxy(root[k]);
    };

    ~deserializer() {};
};

}  // namespace circus