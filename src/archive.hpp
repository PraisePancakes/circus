#pragma once
#include <filesystem>
#include "lexer.hpp"
#include "reader.hpp"
#include "parser.hpp"

namespace circus
{
    class text_archive
    {
    public:
        text_archive(const std::filesystem::path path)
        {
            const std::string input = circus::filesystem::reader__{}(path);
            std::vector<tokens__> tokens = circus::lexer__{}(std::move(input));
            std::vector<circus::object> objects = circus::parser__{}(std::move(tokens));
            // circus::environment__ env = circus::environment__{}(std::move(objects));
        };
        ~text_archive() {};
    };
}