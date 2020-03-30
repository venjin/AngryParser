#ifndef _PARSER_HPP
#define _PARSER_HPP

#include <cassert>
#include <cstdint>

#include <algorithm>
#include <exception>
#include <limits>

#include "../Core/exception.h"
#include "../Core/compilerdetection.h"

NS_BEGINE
inline namespace XML
{

namespace Impl
{

enum class SkipCharType
{
    Space,
    Name,
    AttributeName,
    AttributeValue1,
    AttributeValueNoRef1,
    AttributeValue2,
    AttributeValueNoRef2,
    Text,
    TextNoSpace,
    TextNoRef,
    TextNoSpaceRef,
};

static bool isCharType(char *&p, SkipCharType sct)
{
    if (!p)
    {
        return false;
    }
    auto t = p;
    switch (sct)
    {
    case SkipCharType::Space:
    {
        if (*t && (*t == '\t' || *t == '\n' || *t == '\r' || *t == ' '))
        {
            return true;
        }
    }
    break;
    case SkipCharType::Name:
    {
        if (*t && (*t == '\t' || *t == '\n' || *t == '\r' || *t == ' ' || *t == '/' || *t == '>' || *t == '?'))
        {
            return true;
        }
    }
    break;
    case SkipCharType::AttributeName:
    {
        if (*t && (*t == '\t' || *t == '\n' || *t == '\r' || *t == ' ' || *t == '!' || *t == '/' || *t == '<' || *t == '=' || *t == '>' || *t == '?'))
        {
            return true;
        }
    }
    break;
    case SkipCharType::AttributeValue1:
    {
        if (*t && *t == '"')
        {
            return true;
        }
    }
    break;
    case SkipCharType::AttributeValueNoRef1:
    {
        if (*t && (*t == '"' || *t == '&'))
        {
            return true;
        }
    }
    break;
    case SkipCharType::AttributeValue2:
    {
        if (*t && *t == '\"')
        {
            return true;
        }
    }
    break;
    case SkipCharType::AttributeValueNoRef2:
    {
        if (*t && (*t == '&' || *t == '\"'))
        {
            return true;
        }
    }
    break;
    case SkipCharType::Text:
    {
        if (*t && *t == '<')
        {
            return true;
        }
    }
    break;
    case SkipCharType::TextNoSpace:
    {
        if (*t && (*t == '\t' || *t == '\n' || *t == '\r' || *t == ' ' || *t == '<'))
        {
            return true;
        }
    }
    break;
    case SkipCharType::TextNoRef:
    {
        if (*t && (*t == '&' || *t == '<'))
        {
            return true;
        }
    }
    break;
    case SkipCharType::TextNoSpaceRef:
    {
        if (*t && (*t == '\t' || *t == '\n' || *t == '\r' || *t == ' ' || *t == '&' || *t == '<'))
        {
            return true;
        }
    }
    break;
    default:
        break;
    }
    return false;
}

static size_t skipChar(char *&p, SkipCharType sct = SkipCharType::Space)
{
    if (!p)
    {
        return 0;
    }
    auto t = p;
    switch (sct)
    {
    case SkipCharType::Space:
    {
        while (*t && (*t == '\t' || *t == '\n' || *t == '\r' || *t == ' '))
        {
            ++t;
        }
        size_t length = t - p;
        p = t;
        return length;
    }
    break;
    case SkipCharType::Name:
    {
        while ((*t != '\t') && (*t != '\n') && (*t != '\r') && (*t != ' ') && (*t != '/') && (*t != '>') && (*t != '?'))
        {
            ++t;
        }
        size_t length = t - p;
        p = t;
        return length;
    }
    break;
    case SkipCharType::AttributeName:
    {
        while (*t && (*t != '\t' && *t != '\n' && *t != '\r' && *t != ' ' && *t != '!' && *t != '/' && *t != '<' && *t != '=' && *t != '>' && *t != '?'))
        {
            ++t;
        }
        size_t length = t - p;
        p = t;
        return length;
    }
    break;
    case SkipCharType::AttributeValue1:
    {
        while (*t && *t != '"')
        {
            ++t;
        }
        size_t length = t - p;
        p = t;
        return length;
    }
    break;
    case SkipCharType::AttributeValueNoRef1:
    {
        while (*t && (*t != '"' && *t != '&'))
        {
            ++t;
        }
        size_t length = t - p;
        p = t;
        return length;
    }
    break;
    case SkipCharType::AttributeValue2:
    {
        while (*t && *t != '\"')
        {
            ++t;
        }
        size_t length = t - p;
        p = t;
        return length;
    }
    break;
    case SkipCharType::AttributeValueNoRef2:
    {
        while (*t && (*t != '&' && *t != '\"'))
        {
            ++t;
        }
        size_t length = t - p;
        p = t;
        return length;
    }
    break;
    case SkipCharType::Text:
    {
        while (*t && *t != '<')
        {
            ++t;
        }
        size_t length = t - p;
        p = t;
        return length;
    }
    break;
    case SkipCharType::TextNoSpace:
    {
        while (*t && (*t != '\t' && *t != '\n' && *t != '\r' && *t != ' ' && *t != '<'))
        {
            ++t;
        }
        size_t length = t - p;
        p = t;
        return length;
    }
    break;
    case SkipCharType::TextNoRef:
    {
        while (*t && (*t != '&' && *t != '<'))
        {
            ++t;
        }
        size_t length = t - p;
        p = t;
        return length;
    }
    break;
    case SkipCharType::TextNoSpaceRef:
    {
        while (*t && (*t != '\t' && *t != '\n' && *t != '\r' && *t != ' ' && *t != '&' && *t != '<'))
        {
            ++t;
        }
        size_t length = t - p;
        p = t;
        return length;
    }
    break;
    default:
        break;
    }

    return 0;
}

constexpr unsigned char toDecimalChar(unsigned char t)
{

    return (t >= '0' && t <= '9') ? (t - '0') : 255;
}

constexpr unsigned char toHexadecimalChar(unsigned char t)
{
    return (t >= '0' && t <= '9') ? (t - '0')
                                  : ((t >= 'A' && t <= 'F') ? (t - 'A' + 10)
                                                            : ((t >= 'a' && t <= 'f') ? (t - 'a' + 10) : 255));
}

} // namespace Impl

class XMLParseException : public Exception
{

private:
    std::size_t pos;

public:
    XMLParseException(const StringView&data, std::size_t pos_) : Exception("XMLParseException: " + data), pos(pos_) {}
};

class AngryParser_API XMLParser
{

public:
    enum class Flag : std::uint32_t
    {

        None = 0x00000000,
        TrimSpace = 0x00000001,
        NormalizeSpace = 0x00000002,
        EntityTranslation = 0x00000004,
        ClosingTagValidate = 0x00000008,

        Default = TrimSpace | EntityTranslation,

    };
    friend constexpr bool operator&(Flag a, Flag b)
    {

        return static_cast<std::uint32_t>(a) & static_cast<std::uint32_t>(b);
    }
    friend constexpr Flag operator|(Flag a, Flag b)
    {

        return static_cast<Flag>(static_cast<std::uint32_t>(a) | static_cast<std::uint32_t>(b));
    }

private:
    char *s;
    char *p;

private:
    template <Flag F>
    void parseReference(char *&q)
    {

        switch (p[1])
        {

        case 0:
            throw XMLParseException("Unexpected end of data", p - s);
        case '#':
        {

            if (p[2] == 'x')
            {

                p += 3;
                if (*p == ';')
                    throw XMLParseException("Unexpected ;", p - s);
                std::uint32_t code = 0;
                unsigned char t = 0;
                t = Impl::toHexadecimalChar(*p);
                for (; t != 255;)
                {
                    code = code * 16 + t;
                    ++p;
                    t = Impl::toHexadecimalChar(*p);
                }
                if (*p != ';')
                    throw XMLParseException("Expected ;", p - s);
                ++p;
                // TODO: Code conversion
                *q = code;
                ++q;
            }
            else
            {

                p += 2;
                if (*p == ';')
                    throw XMLParseException("Unexpected ;", p - s);
                std::uint32_t code = 0;
                unsigned char t = 0;
                t = Impl::toDecimalChar(*p);
                for (; t != 255;)
                {
                    code = code * 10 + t;
                    ++p;
                    t = Impl::toDecimalChar(*p);
                }
                if (*p != ';')
                    throw XMLParseException("Expected ;", p - s);
                ++p;
                // TODO: Code conversion
                *q = code;
                ++q;
            }
            return;
        }
        case 'a':
        {

            if (p[2] == 'm' && p[3] == 'p' && p[4] == ';')
            {

                // amp
                p += 5;
                *q = '&';
                ++q;
                return;
            }
            if (p[2] == 'p' && p[3] == 'o' && p[4] == 's' && p[5] == ';')
            {

                // apos
                p += 6;
                *q = '\'';
                ++q;
                return;
            }
            break;
        }
        case 'g':
        {

            if (p[2] == 't' && p[3] == ';')
            {

                // gt
                p += 4;
                *q = '>';
                ++q;
                return;
            }
            break;
        }
        case 'l':
        {

            if (p[2] == 't' && p[3] == ';')
            {

                // lt
                p += 4;
                *q = '<';
                ++q;
                return;
            }
            break;
        }
        case 'q':
        {

            if (p[2] == 'u' && p[3] == 'o' && p[4] == 't' && p[5] == ';')
            {

                // quot
                p += 6;
                *q = '"';
                ++q;
                return;
            }
            break;
        }
        default:
        {

            break;
        }
        }
        throw XMLParseException("Invalid reference", p - s);
    }
    template <Flag F, typename H>
    void parseXMLDeclaration(H & /*handler*/)
    {

        skipChar(p, Impl::SkipCharType::Space);

        // Parse "version"
        if (p[0] != 'v' || p[1] != 'e' || p[2] != 'r' || p[3] != 's' || p[4] != 'i' || p[5] != 'o' || p[6] != 'n')
            throw XMLParseException("Expected version", p - s);
        p += 7;
        skipChar(p, Impl::SkipCharType::Space);
        if (*p != '=')
            throw XMLParseException("Expected =", p - s);
        ++p;
        skipChar(p, Impl::SkipCharType::Space);
        if (*p == '"')
        {

            ++p;
            skipChar(p, Impl::SkipCharType::AttributeValue1);
            if (*p != '"')
                throw XMLParseException("Expected \"", p - s);
        }
        else if (*p == '\'')
        {

            ++p;
            skipChar(p, Impl::SkipCharType::AttributeValue2);
            if (*p != '\'')
                throw XMLParseException("Expected '", p - s);
        }
        else
            throw XMLParseException("Expected \" or '", p - s);
        ++p;

        if (*p != '?' && !isCharType(p, Impl::SkipCharType::Space))
            throw XMLParseException("Unexpected character", p - s);
        skipChar(p, Impl::SkipCharType::Space);

        // Parse "encoding"
        if (p[0] == 'e' && p[1] == 'n' && p[2] == 'c' && p[3] == 'o' && p[4] == 'd' && p[5] == 'i' && p[6] == 'n' && p[7] == 'g')
        {

            p += 8;
            skipChar(p, Impl::SkipCharType::Space);
            if (*p != '=')
                throw XMLParseException("Expected =", p - s);
            ++p;
            skipChar(p, Impl::SkipCharType::Space);
            if (*p == '"')
            {

                ++p;
                skipChar(p, Impl::SkipCharType::AttributeValue1);
                if (*p != '"')
                    throw XMLParseException("Expected \"", p - s);
            }
            else if (*p == '\'')
            {

                ++p;
                skipChar(p, Impl::SkipCharType::AttributeValue2);
                if (*p != '\'')
                    throw XMLParseException("Expected '", p - s);
            }
            else
                throw XMLParseException("Expected \" or '", p - s);
            ++p;
        }

        if (*p != '?' && !isCharType(p, Impl::SkipCharType::Space))
            throw XMLParseException("Unexpected character", p - s);
        skipChar(p, Impl::SkipCharType::Space);

        // Parse "standalone"
        if (p[0] == 's' && p[1] == 't' && p[2] == 'a' && p[3] == 'n' && p[4] == 'd' && p[5] == 'a' && p[6] == 'l' && p[7] == 'o' && p[8] == 'n' && p[9] == 'e')
        {

            p += 10;
            skipChar(p, Impl::SkipCharType::Space);
            if (*p != '=')
                throw XMLParseException("Expected =", p - s);
            ++p;
            skipChar(p, Impl::SkipCharType::Space);
            if (*p == '"')
            {

                ++p;
                skipChar(p, Impl::SkipCharType::AttributeValue1);
                if (*p != '"')
                    throw XMLParseException("Expected \"", p - s);
            }
            else if (*p == '\'')
            {

                ++p;
                skipChar(p, Impl::SkipCharType::AttributeValue2);
                if (*p != '\'')
                    throw XMLParseException("Expected '", p - s);
            }
            else
                throw XMLParseException("Expected \" or '", p - s);
            ++p;
        }

        skipChar(p, Impl::SkipCharType::Space);
        if (p[0] != '?' || p[1] != '>')
            throw XMLParseException("Expected ?>", p - s);
        p += 2;
    }
    template <Flag F, typename H>
    void parseDoctype(H & /*handler*/)
    {

        throw XMLParseException("Not implemented", p - s);
    }
    template <Flag F, typename H>
    void parseComment(H &handler)
    {

        StringView comment(p, 1);
        // Until "-->"
        while (*p && (p[0] != '-' || p[1] != '-' || p[2] != '>'))
            ++p;
        if (!*p)
            throw XMLParseException("Unexpected end of data", p - s);
        comment.setLength(p - comment.getData());
        p += 3;
        handler.comment(comment);
    }
    template <Flag F, typename H>
    void parseProcessingInstruction(H &handler)
    {

        StringView target(p, 1);
        target.setLength(skipChar(p, Impl::SkipCharType::Name));
        if (!target.getLength())
            throw XMLParseException("Expected PI target", p - s);
        if ((p[0] != '?' || p[1] != '>') &&
            !skipChar(p, Impl::SkipCharType::Space))
            throw XMLParseException("Expected white space", p - s);

        StringView content(p, 1);
        // Until "?>"
        while (*p && (p[0] != '?' || p[1] != '>'))
            ++p;
        if (!*p)
            throw XMLParseException("Unexpected end of data", p - s);
        content.setLength(p - content.getData());
        p += 2;

        handler.processingInstruction(target, content);
    }
    template <Flag F, typename H>
    void parseCDATA(H &handler)
    {

        StringView text(p, 1);
        // Until "]]>"
        while (*p && (p[0] != ']' || p[1] != ']' || p[2] != '>'))
            ++p;
        if (!*p)
            throw XMLParseException("Unexpected end of data", p - s);
        text.setLength(p - text.getData());
        p += 3;
        handler.cdata(text);
    }
    template <Flag F, typename H>
    void parseElement(H &handler)
    {

        // Parse element type
        StringView name(p, 1);
        name.setLength(skipChar(p, Impl::SkipCharType::Name));
        if (!name.getLength())
            throw XMLParseException("Expected element type", p - s);
        bool empty = false;
        if (*p == '>')
        {

            ++p;
            handler.startElement(name);
        }
        else if (*p == '/')
        {

            if (p[1] != '>')
                throw XMLParseException("eExpected >", p + 1 - s);
            p += 2;
            handler.startElement(name);
            empty = true;
        }
        else
        {

            ++p;
            handler.startElement(name);
            skipChar(p, Impl::SkipCharType::Space);
            while (!isCharType(p, Impl::SkipCharType::AttributeName))
            {

                // Parse attribute name
                StringView name(p, 1);
                name.setLength(skipChar(p, Impl::SkipCharType::AttributeName));
                if (!name.getLength())
                    throw XMLParseException("Expected attribute name", p - s);
                skipChar(p, Impl::SkipCharType::Space);
                if (*p != '=')
                    throw XMLParseException("Expected =", p - s);
                ++p;
                skipChar(p, Impl::SkipCharType::Space);

                // Parse attribute value
                StringView value;
                if (*p == '"')
                {

                    ++p;
                    value.setData(p, 0);
                    if (F & Flag::EntityTranslation)
                    {

                        auto q = p;
                        while (true)
                        {

                            auto len = skipChar(p, Impl::SkipCharType::AttributeValueNoRef1);
                            if (*p == 0)
                                throw XMLParseException("Unexpected end of data", p - s);
                            if (p != q + len)
                                std::copy(q, q + len, p - len);
                            q += len;
                            if (*p == '&')
                                parseReference<F>(q);
                            else
                                break;
                        }
                        value.setLength(q - value.getData());
                    }
                    else
                    {

                        value.setLength(skipChar(p, Impl::SkipCharType::AttributeValue1));
                        if (*p == 0)
                            throw XMLParseException("Unexpected end of data", p - s);
                    }
                    ++p;
                }
                else if (*p == '\'')
                {

                    ++p;
                    value.setData(p, 0);
                    if (F & Flag::EntityTranslation)
                    {

                        auto q = p;
                        while (true)
                        {

                            auto len = skipChar(p, Impl::SkipCharType::AttributeValueNoRef2);
                            if (*p == 0)
                                throw XMLParseException("Unexpected end of data", p - s);
                            if (p != q + len)
                                std::copy(q, q + len, p - len);
                            q += len;
                            if (*p == '&')
                                parseReference<F>(q);
                            else
                                break;
                        }
                        value.setLength(q - value.getData());
                    }
                    else
                    {

                        value.setLength(skipChar(p, Impl::SkipCharType::AttributeValue2));
                        if (*p == 0)
                            throw XMLParseException("Unexpected end of data", p - s);
                    }
                    ++p;
                }
                else
                    throw XMLParseException("Expected \" or '", p - s);
                handler.attribute(name, value);
                skipChar(p, Impl::SkipCharType::Space);
            }
            if (*p == '>')
            {

                ++p;
            }
            else if (*p == '/')
            {

                if (p[1] != '>')
                    throw XMLParseException("Expected >", p + 1 - s);
                p += 2;
                empty = true;
            }
            else
                throw XMLParseException("Unexpected character", p + 1 - s);
        }
        handler.endAttributes(empty);
        if (!empty)
        {

            bool c = true;
            do
            {

                // Parse text
                if (F & Flag::TrimSpace)
                {
                    skipChar(p, Impl::SkipCharType::Space);
                }
                if (*p != '<')
                {

                    if (F & Flag::EntityTranslation)
                    {

                        if (F & Flag::NormalizeSpace)
                        {

                            StringView text(p, 1);
                            auto q = p;
                            while (true)
                            {

                                auto len = skipChar(p, Impl::SkipCharType::TextNoSpaceRef);
                                if (*p == 0)
                                    throw XMLParseException("Unexpected end of data", p - s);
                                if (p != q + len)
                                    std::copy(p - len, p, q);
                                q += len;
                                if (*p == '&')
                                    parseReference<F>(q);
                                else if (*p != '<')
                                {
                                    skipChar(p, Impl::SkipCharType::Space);
                                    *(q++) = ' ';
                                }
                                else
                                    break;
                            }
                            if (F & Flag::TrimSpace && q[-1] == ' ')
                                --q;
                            text.setLength(q - text.getData());
                            handler.text(text);
                        }
                        else
                        {

                            StringView text(p, 1);
                            auto q = p;
                            while (true)
                            {

                                auto len = skipChar(p, Impl::SkipCharType::TextNoRef);
                                if (*p == 0)
                                    throw XMLParseException("Unexpected end of data", p - s);
                                if (p != q + len)
                                    std::copy(p - len, p, q);
                                q += len;
                                if (*p == '&')
                                    parseReference<F>(q);
                                else
                                    break;
                            }
                            --q;
                            if (F & Flag::TrimSpace)
                            {
                                while (isCharType(q, Impl::SkipCharType::Space))
                                {
                                    --q;
                                }
                            }
                            ++q;
                            text.setLength(q - text.getData());
                            handler.text(text);
                        }
                    }
                    else
                    {

                        if (F & Flag::NormalizeSpace)
                        {

                            StringView text(p, 1);
                            auto q = p;
                            while (true)
                            {

                                auto len = skipChar(p, Impl::SkipCharType::TextNoSpace);
                                if (*p == 0)
                                    throw XMLParseException("Unexpected end of data", p - s);
                                if (p != q + len)
                                    std::copy(p - len, p, q);
                                q += len;
                                if (*p != '<')
                                {
                                    skipChar(p, Impl::SkipCharType::Space);
                                    *(q++) = ' ';
                                }
                                else
                                    break;
                            }
                            --q;
                            if (F & Flag::TrimSpace)
                            {
                                while (isCharType(q, Impl::SkipCharType::Space))
                                {
                                    --q;
                                }
                            }
                            ++q;
                            text.setLength(q - text.getData());
                            handler.text(text);
                        }
                        else
                        {

                            StringView text(p, 1);
                            skipChar(p, Impl::SkipCharType::Text);
                            if (*p == 0)
                                throw XMLParseException("Unexpected end of data", p - s);
                            auto q = p - 1;
                            if (F & Flag::TrimSpace)
                            {
                                while (isCharType(q, Impl::SkipCharType::Space))
                                {
                                    --q;
                                }
                            }
                            ++q;
                            text.setLength(q - text.getData());
                            handler.text(text);
                        }
                    }
                }

                ++p;
                switch (*p)
                {

                case '!':
                {

                    ++p;
                    if (p[0] == '-' && p[1] == '-')
                    {

                        p += 2;
                        parseComment<F>(handler);
                    }
                    else if (p[0] == '[' && p[1] == 'C' && p[2] == 'D' && p[3] == 'A' && p[4] == 'T' && p[5] == 'A' && p[6] == '[')
                    {

                        // "[CDATA["
                        p += 7;
                        parseCDATA<F>(handler);
                    }
                    else
                        throw XMLParseException("Unexpected character", p - s);
                    break;
                }
                case '/':
                {

                    ++p;
                    if (F & Flag::ClosingTagValidate)
                    {

                        StringView endName(p, 1);
                        skipChar(p, Impl::SkipCharType::Name);
                        endName.setLength(p - endName.getData());
                        skipChar(p, Impl::SkipCharType::Space);
                        if (*p != '>')
                            throw XMLParseException("Expected >", p - s);
                        ++p;
                        handler.endElement(endName);
                    }
                    else
                    {

                        StringView endName(p, name.getLength());
                        if (endName != name)
                            throw XMLParseException("Unmatch element type", p - s);
                        p += name.getLength();
                        skipChar(p, Impl::SkipCharType::Space);
                        if (*p != '>')
                            throw XMLParseException("Expected >", p - s);
                        ++p;
                        handler.endElement(endName);
                    }
                    c = false;
                    break;
                }
                case '?':
                {

                    ++p;
                    parseProcessingInstruction<F>(handler);
                    break;
                }
                default:
                {

                    parseElement<F>(handler);
                    break;
                }
                }

            } while (c);
        }
    }

public:
    XMLParser() = default;

    template <Flag F = Flag::Default, typename H>
    void parse(char *data, H &handler)
    {
        assert(data);

        s = data;
        p = data;
        handler.startDocument();

        // Parse BOM
        if (static_cast<unsigned char>(p[0]) == 0xEF &&
            static_cast<unsigned char>(p[1]) == 0xBB &&
            static_cast<unsigned char>(p[2]) == 0xBF)
        {

            p += 3;
        }

        // Parse XML declaration '\t', '\n', '\r', ' '
        if (p[0] == '<' && p[1] == '?' && p[2] == 'x' && p[3] == 'm' && p[4] == 'l' && (p[5] == '\t' || p[5] == '\n' || p[5] == '\r' || p[5] == ' '))
        {

            // "<?xml "
            p += 6;
            parseXMLDeclaration<F>(handler);
        }
        while (true)
        {

            skipChar(p, Impl::SkipCharType::Space);
            if (!*p)
                break;
            else if (*p == '<')
            {

                ++p;
                if (*p == '!')
                {

                    ++p;
                    if (p[0] == '-' && p[1] == '-')
                    {

                        p += 2;
                        parseComment<F>(handler);
                    }
                    else if (p[0] == 'D' && p[1] == 'O' && p[2] == 'C' && p[3] == 'T' && p[4] == 'Y' && p[5] == 'P' && p[6] == 'E')
                    {

                        // "DOCTYPE"
                        p += 7;
                        parseDoctype<F>(handler);
                    }
                    else
                        throw XMLParseException("Unexpected character", p - s);
                }
                else if (*p == '?')
                {

                    ++p;
                    parseProcessingInstruction<F>(handler);
                }
                else
                {

                    parseElement<F>(handler);
                }
            }
            else
                throw XMLParseException("Expected <", p - s);
        }

        handler.endDocument();
    }
};

} // namespace XML
NS_END

#endif
