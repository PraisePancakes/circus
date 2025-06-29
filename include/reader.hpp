#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <filesystem>

namespace circus::filesystem
{
    class reader__
    {
        std::string _contents;
        std::string _path;

    public:
        reader__() : _contents{}, _path{} {};

        std::string operator()(std::istream &is)
        {
            std::string contents((std::istreambuf_iterator<char>(is)),
                                 std::istreambuf_iterator<char>());
            _contents = std::move(contents);
            return _contents;
        };

        [[nodiscard]] std::size_t size() const noexcept
        {
            return _contents.size();
        };

        [[nodiscard]] const std::string &get_file_path() const noexcept { return _path; };
        [[nodiscard]] const std::string &get_file_contents() const noexcept { return _contents; };
    };
};