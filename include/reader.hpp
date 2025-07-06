#pragma once
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace circus::filesystem {
class reader__ {
    std::string _contents;

   public:
    reader__() : _contents{} {};

    std::string operator()(std::istream &is) {
        std::string contents((std::istreambuf_iterator<char>(is)),
                             std::istreambuf_iterator<char>());
        _contents = std::move(contents);
        return _contents;
    };

    [[nodiscard]] std::size_t size() const noexcept {
        return _contents.size();
    };

    [[nodiscard]] const std::string &get_file_contents() const noexcept { return _contents; };
};
};  // namespace circus::filesystem