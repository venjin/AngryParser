#ifndef _EXCEPTION_HPP
#define _EXCEPTION_HPP

#include <exception>
#include <memory>

#include "compilerdetection.h"

#include "string.h"

NS_BEGINE

inline namespace Core
{

class Exception : public std::exception
{
private:
    std::shared_ptr<const StringView> data;

public:
    Exception(const StringView&data_) : data(std::make_shared<const StringView>(data_)) {}

    const char *what() const noexcept override { return data->getData(); }
};

class InvalidArgumentException : public Exception
{

public:
    InvalidArgumentException(const StringView&data) : Exception("InvalidArgumentException: " + data) {}
};

class SystemException : public Exception
{

public:
    SystemException(const StringView&data) : Exception("SystemException: " + data) {}
};

class IOException : public Exception
{

public:
    IOException(const StringView&data) : Exception("IOException: " + data) {}
};

} // namespace Core
NS_END

#endif
