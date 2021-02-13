#include <mutex>
#include "SymbolTable.hpp"

using namespace std;

namespace CCW::Tula {

    static mutex gSymbolTableLock;
    static SymbolTable* gSymbolTable;


    class SymbolTable::Bucket : public Noncopyable {
    public:
        void push(const SymbolPtr &symbol) {
            lock_guard<mutex> _(lock);
            symbols.push_back(symbol);
        }

        bool contains(const SymbolPtr &symbol) {
            lock_guard<mutex> _(lock);
            return find_if(symbols.begin(), symbols.end(), [&symbol](const auto &sym) {
                return sym->equals(symbol);
            }) != symbols.end();
        }

    private:
        mutex lock;
        vector<SymbolPtr> symbols;
    };

    void SymbolTable::init() {
        gSymbolTable = new SymbolTable();
    }

    void SymbolTable::release() {
        delete gSymbolTable;
    }

    bool SymbolTable::putSymbol(const SymbolPtr &symbol) {
        if (contains(symbol)) {
            return false;
        }
        lock_guard<mutex> _(gSymbolTableLock);
        Symbol::Hash hash = symbol->hash();
        auto found = gSymbolTable->table.find(hash);
        if (found == gSymbolTable->table.end()) {
            auto bucket = make_shared<SymbolTable::Bucket>();
            bucket->push(symbol);
            gSymbolTable->table[hash] = bucket;
        } else {
            found->second->push(symbol);
        }
        return true;
    }

    bool SymbolTable::contains(const SymbolPtr &symbol) {
        if (auto foundBucket = findBucketByHash(symbol->hash())) {
            auto bucket = *foundBucket;
            return bucket->contains(symbol);
        }
        return false;
    }

    optional<SymbolTable::BucketPtr> SymbolTable::findBucketByHash(Symbol::Hash hash) {
        lock_guard<mutex> _(gSymbolTableLock);
        const auto &found = gSymbolTable->table.find(hash);
        if (found != gSymbolTable->table.end()) {
            return found->second;
        } else {
            return nullopt;
        }
    }

}
