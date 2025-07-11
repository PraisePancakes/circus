#pragma once
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace circus::filesystem {

/**
 * @class reader__
 * @brief Utility class to read contents from input streams into a string.
 *
 * Provides an interface to load the entire contents of an std::istream
 * into an internal string buffer for later access.
 */
class reader__ {
    /// Stores the contents read from the input stream
    std::string _contents;

   public:
    /**
     * @brief Default constructor initializes an empty content buffer.
     */
    reader__() : _contents{} {};

    /**
     * @brief Reads entire contents of the provided input stream into internal buffer.
     *
     * @param is Input stream to read from.
     * @return A reference to the internal string containing the full content.
     */
    std::string operator()(std::istream &is) {
        std::string contents((std::istreambuf_iterator<char>(is)),
                             std::istreambuf_iterator<char>());
        _contents = std::move(contents);
        return _contents;
    };

    /**
     * @brief Get the size in bytes of the stored content.
     *
     * @return Size of the content string.
     */
    [[nodiscard]] std::size_t size() const noexcept {
        return _contents.size();
    };

    /**
     * @brief Provides read-only access to the stored file contents.
     *
     * @return Const reference to the internal content string.
     */
    [[nodiscard]] const std::string &get_file_contents() const noexcept { return _contents; };
};

};  // namespace circus::filesystem
