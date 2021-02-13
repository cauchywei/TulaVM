#pragma once

#include "Symbol.hpp"

#include <memory>

namespace CCW::Tula {
    class Klass : public Interface {
    public:
        using Ptr = std::shared_ptr<Klass>;

        virtual const SymbolPtr &name() = 0;

    private:

    };
}
