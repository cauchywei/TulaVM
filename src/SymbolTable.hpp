#pragma once

#include "Symbol.hpp"

#include <map>
#include <vector>
#include <optional>

namespace CCW::Tula {

    using SymbolHash =  int32_t;

    using SymbolList =  std::vector<Symbol::Ptr>;

    using SymbolListPtr =  std::shared_ptr<SymbolList>;


    class SymbolTable : public Noncopyable {
    public:

        class Bucket;

        using BucketPtr = std::shared_ptr<Bucket>;

        static bool putSymbol(const Symbol::Ptr &symbol);

        static bool contains(const Symbol::Ptr &symbol);

        static std::optional<BucketPtr> findBucketByHash(Symbol::Hash hash);

    private:
        friend class VM;

        static void init();

        static void release();

    private:
        std::map<SymbolHash, BucketPtr> table;
    };
}


