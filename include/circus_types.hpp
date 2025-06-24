#pragma once

namespace circus
{
    struct tokens__
    {
        using literal_variant_t = std::variant<unsigned char, std::string, int, float>;
        using location_t = std::pair<std::size_t, std::size_t>;
        enum class TYPE : unsigned char
        {
            // RESERVED UNITS
            TK_QUOTE_DOUBLE = '\"',
            TK_PAREN_L = '(',
            TK_PAREN_R = ')',
            TK_COMMA = ',',
            TK_COLON = ':',
            TK_BRACE_L = '[',
            TK_BRACE_R = ']',
            TK_DOLLA = '$',
            TK_CURL_L = '{',
            TK_CURL_R = '}',
            TK_QUOTE_SINGLE = '\'',
            TK_STAR = '*',
            TK_SLASH = '/',

            TK_EOF = '\0',

            // NON RESERVED UNITS
            TK_SPACE = ' ',
            TK_NEWLINE = '\n',
            TK_LITERAL_INT = 0xFE,
            TK_LITERAL_FLOAT = 0xFD,
            TK_IDENTIFIER = 0xFC,
            TK_LITERAL_STRING = 0xFB,

            // unknown
            TK_UNKNOWN = 0xFF

        } _token_type;

    protected:
        literal_variant_t _literal;
        location_t _location;

    public:
#if CIRCUS_DEBUG_PEDANTIC__
        static std::string type_to_string(TYPE type) noexcept
        {
            switch (type)
            {
            case tokens__::TYPE::TK_QUOTE_DOUBLE:
                return "[TK_QUOTE_DOUBLE]";
            case tokens__::TYPE::TK_PAREN_L:
                return "[TK_PAREN_L]";
            case tokens__::TYPE::TK_PAREN_R:
                return "[TK_PAREN_R]";
            case tokens__::TYPE::TK_COMMA:
                return "[TK_COMMA]";
            case tokens__::TYPE::TK_COLON:
                return "[TK_COLON]";
            case tokens__::TYPE::TK_BRACE_L:
                return "[TK_BRACE_L]";
            case tokens__::TYPE::TK_BRACE_R:
                return "[TK_BRACE_R]";
            case tokens__::TYPE::TK_DOLLA:
                return "[TK_DOLLA]";
            case tokens__::TYPE::TK_CURL_L:
                return "[TK_CURL_L]";
            case tokens__::TYPE::TK_CURL_R:
                return "[TK_CURL_R]";
            case tokens__::TYPE::TK_QUOTE_SINGLE:
                return "[TK_QUOTE_SINGLE]";
            case tokens__::TYPE::TK_STAR:
                return "[TK_STAR]";
            case tokens__::TYPE::TK_SLASH:
                return "[TK_SLASH]";
            case tokens__::TYPE::TK_NEWLINE:
                return "[TK_NEWLINE]";
            case tokens__::TYPE::TK_EOF:
                return "[TK_EOF]";
            case tokens__::TYPE::TK_LITERAL_STRING:
                return "[TK_LITERAL_STRING]";
            case tokens__::TYPE::TK_LITERAL_INT:
                return "[TK_LITERAL_INT]";
            case tokens__::TYPE::TK_LITERAL_FLOAT:
                return "[TK_LITERAL_FLOAT]";
            case tokens__::TYPE::TK_IDENTIFIER:
                return "[TK_IDENTIFIER]";
            default:
                return "[UNRECOGNIZED TOKEN]";
            };
        };

        static unsigned char type_to_literal(TYPE type) noexcept
        {
            return (unsigned char)(type);
        };

        void print_token() const noexcept
        {
            std::cout << "TOKEN TYPE ID (" << static_cast<int>(_token_type) << ")" << " [" << type_to_string(_token_type) << "]\n";
            std::cout << "location (row, col) < " << _location.first << " , " << _location.second << " > ";
            std::visit(internal::visitor{[](unsigned char c)
                                         { std::cout << "[UCHAR] " << c << std::endl; },
                                         [](std::string s)
                                         {
                                             std::cout << "[STRING] " << s << std::endl;
                                         },
                                         [](int i)
                                         { std::cout << "[INT] " << i << std::endl; },
                                         [](float f)
                                         { std::cout << "[FLOAT] " << f << std::endl; }},
                       _literal);
        };
#endif

        tokens__(TYPE type, literal_variant_t lit, location_t loc)
            : _token_type(type), _literal(lit), _location(loc) {};
    };
};