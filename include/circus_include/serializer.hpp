#pragma once
#include <filesystem>
#include <variant>

#include "circus_traits.hpp"
#include "token.hpp"
#include "utils/to_chars.hpp"

namespace circus {
/**
 * @def CIRCUS_ENTRY(VAR)
 * @brief Macro to simplify serialization by automatically pairing a variable with its name.
 */
#define CIRCUS_ENTRY(VAR) std::make_pair(#VAR, VAR)

/**
 * @class serializer
 * @brief A flexible serializer for C++ objects, primitives, STL containers, and user-defined types.
 *
 * The `serializer` emits a structured, human-readable output inspired by formats like JSON or YAML,
 * but uses custom token syntax and compile-time traits to determine how to serialize different types.
 *
 * @tparam OStreamT Output stream type (must derive from `std::ostream`)
 */

template <typename OStreamT>
    requires(std::is_base_of_v<std::ostream, OStreamT>)
class serializer {
    OStreamT &stream;
    using TK = tokens__::TYPE;

    /**
     * @brief Handles the serialization of basic streamable literals.
     * Adds quotes for string-like types.
     */
    template <std::size_t I, std::size_t Max, traits::OutStreamableLiteral Arg>
    void handle_value_type(Arg &&arg) {
        if constexpr (traits::StringLike<Arg>) {
            stream << tokens__::to_literal(TK::TK_QUOTE_DOUBLE) << arg << tokens__::to_literal(TK::TK_QUOTE_DOUBLE);
        } else {
            stream << arg;
        }
        stream << tokens__::to_literal(TK::TK_SPACE);
    }
    /**
     * @brief Handles the serialization of user-defined types with a `serialize` method.
     */
    template <std::size_t I, std::size_t Max, traits::Serializable Arg>
    void handle_value_type(Arg &&arg) {
        stream << tokens__::to_literal(TK::TK_CURL_L);
        arg.serialize(*this);
        stream << tokens__::to_literal(TK::TK_CURL_R);
    }
    /**
     * @brief Handles serialization of vector-like types using `{ elem1, elem2, ... }` format.
     */
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
    /**
     * @brief Handles serialization of generic container types by iterating and calling handle_pair().
     */
    template <std::size_t I, std::size_t Max, traits::Container Arg>
    void handle_value_type(Arg &&arg) {
        for (auto &a : arg) {
            handle_pair(a);
        }
    }
    /**
     * @brief Handles serialization of a key-value pair.
     */
    template <std::size_t I, std::size_t Max, traits::PairSerializable Arg>
    void handle_pair(Arg &&arg) {
        stream << tokens__::to_literal(TK::TK_DOLLA) << arg.first << tokens__::to_literal(TK::TK_COLON);
        handle_value_type<I - 1, Max>(arg.second);
        if constexpr (I < Max - 1) {
            stream << tokens__::to_literal(TK::TK_COMMA);
        }
    }
    /**
     * @brief Ensures unnamed arguments are made into serializable key-value pairs using numeric labels.
     */
    template <std::size_t I, typename Arg>
    constexpr static auto make_pair_serializable(Arg &&arg) {
        if constexpr (!traits::PairSerializable<Arg>) {
            return std::make_pair(circus::utils::num_to_string<I>::value, std::forward<Arg>(arg));
        } else {
            return std::forward<Arg>(arg);
        }
    }
    /**
     * @brief Dispatches serialization for variadic arguments.
     */
    template <std::size_t... Is, typename... Args>
    void handler(std::index_sequence<Is...>, Args &&...args) {
        constexpr std::size_t MAXARG = sizeof...(Args);
        (handle_pair<Is, MAXARG>(make_pair_serializable<Is>(std::forward<Args>(args))), ...);
    }
    bool init = false;

   public:
    /**
     * @brief Constructs a serializer with the given output stream.
     * @param s Output stream reference (e.g., std::ostringstream, std::ofstream)
     */
    serializer(OStreamT &s) : stream(s) {
        stream << "$root : {";
    };
    /**
     * @brief Serializes the given arguments using function-call syntax.
     *
     * Example:
     * @code
     * serializer s(os);
     * s(CIRCUS_ENTRY(x), CIRCUS_ENTRY(y));
     * @endcode
     */
    template <typename... Args>
    void operator()(Args &&...args) & {
        handler(std::index_sequence_for<Args...>{}, std::forward<Args>(args)...);
    }
    /**
     * @brief Serializes the given arguments using stream-like syntax.
     *
     * Example:
     * @code
     * circus::serializer s(os);
     * s << CIRCUS_ENTRY(x) << CIRCUS_ENTRY(y);
     * @endcode
     *
     * @return Reference to the serializer
     */
    template <typename... Args>
    serializer &operator<<(Args &&...args) {
        handler(std::index_sequence_for<Args...>{}, std::forward<Args>(args)...);
        return *this;
    }

    ~serializer() { stream << '}'; };
};
}  // namespace circus