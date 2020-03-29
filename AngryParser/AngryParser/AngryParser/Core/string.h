#ifndef STRING_HPP
#define STRING_HPP

#include <cstddef>

#include <algorithm>
#include <iostream>
#include <string>

#include "compilerdetection.h"

NS_BEGINE
inline namespace Core
{

static int compareChar(const char *begin1, const char *end1, const char *begin2, const char *end2) noexcept
{

    using U = std::make_unsigned_t<char>;
    for (auto p = begin1, q = begin2;; ++p, ++q)
    {

        bool m1 = p == end1, m2 = q == end2;
        if (m1 || m2)
            return m2 - m1;
        char c1 = *p, c2 = *q;
        if (c1 != c2)
            return U(c1) < U(c2) ? -1 : 1;
    }
}

static std::size_t getCharLength(const char *str) noexcept
{

    const char *end = str;
    while (*end != char())
        ++end;
    return end - str;
}

class AngryParser_API StringView
{

public:
    using CharType = char;

    using Iterator = const CharType *;
    using ConstIterator = Iterator;

private:
    const CharType *data;
    std::size_t length;

public:
    constexpr StringView() noexcept : data(nullptr), length(0) {}
    constexpr StringView(const CharType *data_, std::size_t length_) noexcept : data(data_), length(length_) {}
    StringView(const CharType *data_) noexcept : StringView(data_, getCharLength(data_)) {}
    constexpr StringView(const CharType *b, const CharType *e) noexcept : data(b), length(e - b) {}
    constexpr StringView(const StringView &src) noexcept = default;
    ~StringView() = default;

    StringView &operator=(const StringView &src) = default;

    friend StringView operator+(const CharType *a, const StringView &b)
    {
        StringView c;
		char cp[1024] = "";
		c.setData(cp);
        auto length = getCharLength(a);
        auto length_ = b.getLength();
        c.setLength(length + length_);
        auto data = a;
        auto data_ = b.getData();
        auto datac = c.getData();
        std::copy(data, data + length, datac);
        std::copy(data_, data_ + length_, datac + length);
        return c;
    }

    const CharType &operator[](std::size_t index) const noexcept { return data[index]; }

    friend bool operator==(const StringView &a, const StringView &b) noexcept
    {

        if (a.length != b.length)
            return false;
        for (auto p = a.data, q = b.data, end = p + a.length; p != end; ++p, ++q)
            if (*p != *q)
                return false;
        return true;
    }
    friend bool operator!=(const StringView &a, const StringView &b) noexcept { return !(a == b); }
    friend bool operator<(const StringView &a, const StringView &b) noexcept { return compareChar(a.data, a.data + a.length, b.data, b.data + b.length) < 0; }
    friend bool operator>(const StringView &a, const StringView &b) noexcept { return compareChar(a.data, a.data + a.length, b.data, b.data + b.length) > 0; }
    friend bool operator<=(const StringView &a, const StringView &b) noexcept { return compareChar(a.data, a.data + a.length, b.data, b.data + b.length) <= 0; }
    friend bool operator>=(const StringView &a, const StringView &b) noexcept { return compareChar(a.data, a.data + a.length, b.data, b.data + b.length) >= 0; }

    friend std::basic_ostream<CharType> &operator<<(std::basic_ostream<CharType> &stream, const StringView &sv)
    {

        stream.write(sv.getData(), sv.getLength());
        return stream;
    }

    const CharType *getData() const noexcept { return data; }
    CharType *getData() noexcept { return (char *)data; }
    void setData(const CharType *data_) noexcept { setData(data_, getCharLength(data_)); }
    void setData(const CharType *data_, std::size_t length_) noexcept { data = data_, length = length_; }
    std::size_t getLength() const noexcept { return length; }
    void setLength(std::size_t length_) noexcept { length = length_; }

    bool isEmpty() const noexcept { return !length; }

    Iterator begin() const noexcept { return data; }
    Iterator end() const noexcept { return data + length; }
};

inline namespace StringViewLiteral
{

constexpr StringView operator"" _sv(const char *data, std::size_t length) noexcept { return {data, length}; }

} // namespace StringViewLiteral

} // namespace Core
NS_END

#endif
