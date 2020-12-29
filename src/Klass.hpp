#pragma once

#include "Symbol.hpp"

#include <memory>

namespace ccw::tula {
    class Klass : public Interface {
    public:
        using Ptr = std::shared_ptr<Klass>;

        virtual const Symbol::Ptr &name() = 0;

    private:

    };
}
