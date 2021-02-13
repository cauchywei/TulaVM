#pragma once

#include "Klass.hpp"
#include "Symbol.hpp"

#include <memory>
#include <mutex>
#include <shared_mutex>
#include <vector>

namespace CCW::Tula {

    class VM;

    class ClazzLoader : public Interface {
    public:
        using Ptr =  std::shared_ptr<ClazzLoader>;

        virtual Klass::Ptr defineClass(const std::string &clazzPath) = 0;

        virtual Klass::Ptr loadClass(const SymbolPtr &clazz) = 0;

        virtual Klass::Ptr findClass(const SymbolPtr &clazz) = 0;
    };

    class BootstrapClassLoader : public ClazzLoader {
    public:
        friend class VM;

        BootstrapClassLoader(VM *vm, std::string libPath);

        Klass::Ptr defineClass(const std::string &clazzPath) override;

        Klass::Ptr loadClass(const SymbolPtr &clazz) override;

        Klass::Ptr findClass(const SymbolPtr &clazz) override;

        Klass::Ptr findLoadedKlass(const SymbolPtr &clazz);

    private:
        VM *vm;
        std::string libPath;

        std::shared_timed_mutex clazzMutex;
        std::vector<Klass::Ptr> loadedClazzs;
    };
}