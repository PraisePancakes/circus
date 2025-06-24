#pragma once

namespace circus
{
    namespace internal
    {
        template <typename... fs>
        struct visitor : fs...
        {
            using fs::operator()...;
        };
    }

}