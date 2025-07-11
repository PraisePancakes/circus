#pragma once
#include <iostream>

#include "lexer.hpp"
#include "parser.hpp"

namespace circus {
/**
 * @file deserializer.hpp
 * @brief Defines the circus::deserializer class for reading structured text data into C++ objects.
 */

/**
 * @class deserializer
 * @brief Parses structured circus-formatted input and exposes access to the parsed data.
 *
 * This class reads an input stream, lexes and parses it into a structured tree (`circ_variable`),
 * and provides safe, nested access through `operator[]` and type-safe retrieval using `.value<T>()`.
 *
 * @tparam IStreamT Input stream type (must derive from `std::istream`)
 */
template <typename IStreamT>
    requires(std::is_base_of_v<std::istream, IStreamT>)
class deserializer {
    std::unordered_map<std::string, circ_variable> root;
    /**
     * @class circ_safe_proxy
     * @brief A proxy that wraps access to a circ_variable and allows nested lookups and safe casting.
     *
     * Used to chain access like `archive["user"]["name"]` and extract typed values.
     */
    class circ_safe_proxy {
        circ_variable& var;

       public:
        /**
         * @brief Constructs a proxy from a circ_variable reference.
         */
        circ_safe_proxy(circ_variable& v) : var(v) {};

        /**
         * @brief Allows nested access via key lookup.
         * @param key Key to access nested circ_variable
         * @return Another circ_safe_proxy
         */
        circ_safe_proxy operator[](const std::string& key) {
            return circ_safe_proxy(var[key]);
        };
        /**
         * @brief Allows nested access via key lookup using a C-string.
         * @param key Key to access nested circ_variable
         * @return Another circ_safe_proxy
         */
        circ_safe_proxy operator[](const char* key) {
            return circ_safe_proxy(var[std::string(key)]);
        }
        /**
         * @brief Explicitly retrieves the value of the variable as a reference of type T.
         * @tparam T Type to extract (must match the underlying variant type)
         * @return Reference to the value
         */
        template <typename T>
        T& value() {
            return std::get<T>(var.value);
        }
        /**
         * @brief Implicit conversion to value type T (e.g., for use in expressions).
         * @tparam T Type to convert to
         */
        template <typename T>
        operator T() {
            return std::get<T>(var.value);
        }
    };

   public:
    using circ_object = std::unordered_map<std::string, circ_variable>;

    /**
     * @brief Constructs a deserializer from an input stream.
     *
     * This constructor will:
     * 1. Read the full input into a string.
     * 2. Tokenize the source text.
     * 3. Parse the token stream into a structured root object.
     *
     * @param is Input stream containing serialized circus-formatted data.
     */
    deserializer(IStreamT& is) {
        std::string source = circus::filesystem::reader__{}(is);
        std::vector<circus::tokens__> tokens = circus::lexer__{}(std::move(source));
        std::unordered_map<std::string, circ_variable> rt = circus::parser__{}(std::move(tokens));
        root = std::move(rt);
    };
    /**
     * @brief Destructor (default).
     */
    circ_safe_proxy operator[](const std::string& k) {
        return circ_safe_proxy(root[k]);
    };

    ~deserializer() = default;
};

}  // namespace circus