#include "ClazzLoader.hpp"
#include "classfile/ClassFileParser.hpp"
#include "Error.hpp"

#include <fstream>
#include <utility>

namespace CCW::Tula {

    BootstrapClassLoader::BootstrapClassLoader(VM *vm, std::string libPath) : vm(vm), libPath(std::move(libPath)) {}

    Klass::Ptr BootstrapClassLoader::defineClass(const std::string &clazzPath) {
        std::fstream f(clazzPath);
        if (!f.is_open()) {
            // TODO throw class not found
            return nullptr;
        }
        f.seekg(0, std::ios::end);
        size_t size = f.tellg();
        uint8_t buffer[size];
        f.seekg(0);
        f.read(reinterpret_cast<char *>(buffer), size);
        return ClassFileParser::parse(buffer, size);
    }

    Klass::Ptr BootstrapClassLoader::loadClass(const Symbol::Ptr &clazz) {
        return Tula::Klass::Ptr();
    }

    Klass::Ptr BootstrapClassLoader::findClass(const Symbol::Ptr &clazz) {
        return Tula::Klass::Ptr();
    }

    Klass::Ptr BootstrapClassLoader::findLoadedKlass(const Symbol::Ptr &clazz) {
        std::shared_lock<std::shared_timed_mutex> _{clazzMutex};
        auto it = std::find_if(std::begin(loadedClazzs), std::end(loadedClazzs),
                               [&clazz](const Klass::Ptr &it) { return it->name() == clazz; });
        if (it == std::end(loadedClazzs)) {
            return nullptr;
        } else {
            return *it;
        }
    }

}