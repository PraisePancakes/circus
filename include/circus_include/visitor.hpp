#pragma once

namespace circus {
namespace internal {

/**
 * @brief Helper struct to combine multiple callable objects into a single visitor.
 *
 * This utility enables visiting variants with multiple lambdas or functors
 * by inheriting from all provided callable types and using their operator().
 *
 * @tparam fs Parameter pack of callable types.
 */
template <typename... fs>
struct visitor : fs... {
    using fs::operator()...;
};

/**
 * @brief Deduction guide for visitor to deduce template arguments from constructor arguments.
 *
 * @tparam fs Parameter pack of callable types.
 * @param fs Instances of callable objects.
 */
template <typename... fs>
visitor(fs...) -> visitor<fs...>;

}  // namespace internal
}  // namespace circus
