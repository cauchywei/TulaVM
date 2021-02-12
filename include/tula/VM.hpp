#pragma once

#include <CCW/Base.hpp>
#include <string>

namespace CCW::Tula {
    class BootstrapClassLoader;

    class VM : public Noncopyable {
    public:
        static VM* current();
    public:

        explicit VM(std::string libPath, std::string initializeClazzPath);

        void start();

        virtual ~VM();

    private:
        const std::string libPath;
        const std::string initializeClazzPath;
        std::shared_ptr<BootstrapClassLoader> bootstrapClazzLoader;
    };
}