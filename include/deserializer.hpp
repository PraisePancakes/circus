#pragma once
#include <iostream>

namespace circus
{
    template <typename IStreamT>
        requires(std::is_base_of_v<std::istream, IStreamT>)
    class deserializer
    {
        
    };

}