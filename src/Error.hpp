
#pragma once

#include <string>
#include <utility>

namespace CCW::Tula {
    class Error : public std::exception {
    public:
        Error() : message() {}

        explicit Error(std::string message) : message(std::move(message)) {}


        const char *what() const noexcept override {
            return message.c_str();
        }

    protected:
        std::string message;
    };

    class LinkageError : public Error {
    public:
        LinkageError() : Error() {};

        explicit LinkageError(const std::string &message) : Error(message) {}
    };

    class ClassFormatError : public LinkageError {
    public:
        ClassFormatError() : LinkageError() {}

        explicit ClassFormatError(const std::string &message) : LinkageError(message) {}
    };
}
