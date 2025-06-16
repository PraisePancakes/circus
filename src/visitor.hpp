#pragma once

namespace circus
{
    template <typename... fs>
    struct visitor : fs...
    {
        using fs::operator()...;
    };

}