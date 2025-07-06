#pragma once

namespace circus {
namespace internal {
template <typename... fs>
struct visitor : fs... {
    using fs::operator()...;
};

template <typename... fs>
visitor(fs...) -> visitor<fs...>;
}  // namespace internal

}  // namespace circus