#include "tula/VM.hpp"
#include "ClazzLoader.hpp"
#include "SymbolTable.hpp"

namespace CCW::Tula {
    // TODO add lock
    static VM *gVM = nullptr;

    VM::VM(std::string libPath, std::string initializeClazzPath) : libPath(std::move(libPath)),
                                                                   initializeClazzPath(std::move(initializeClazzPath)) {
        SymbolTable::init();
        bootstrapClazzLoader = std::make_shared<BootstrapClassLoader>(this, this->libPath);
        gVM = this;
    }

    void VM::start() {
        auto initializeClazz = bootstrapClazzLoader->defineClass(initializeClazzPath);
        if(initializeClazz == nullptr) {
            fprintf(stderr, "Class not found at %s", initializeClazzPath.c_str());
            exit(-1);
        }
    }

    VM *VM::current() {
        return gVM;
    }

    VM::~VM() {
        SymbolTable::release();
        if (gVM == this) {
            gVM = nullptr;
        }
    }
}