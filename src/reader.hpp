#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
namespace circus::filesystem
{
    class reader__
    {
        std::string _contents;
        std::string _path;

    public:
        reader__() : _contents{}, _path{} {};
        std::string operator()(const std::string &fp)
        {
            std::ifstream fs(fp, std::ios_base::in);
            if (fs)
            {
                std::ostringstream ss;
                ss << fs.rdbuf();
                _contents = ss.str();
            }
            else
            {
                std::cerr << "FILE FAILED TO OPEN" << std::endl;
            }
            fs.close();
            _path = fp;
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