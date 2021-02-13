#pragma once

#include <CCW/Base.hpp>
#include <memory>

namespace CCW::Tula {

    class Symbol : public Noncopyable, public std::enable_shared_from_this<Symbol> {
    public:
        using Hash = uint32_t;

        Symbol(const uint8_t *bytes, size_t len);

        virtual ~Symbol();

        static inline std::shared_ptr<Symbol> create(const uint8_t *bytes, size_t len) {
            return std::make_shared<Symbol>(bytes, len);
        }

        static inline std::shared_ptr<Symbol> create(const char *cstr) {
            return std::make_shared<Symbol>(reinterpret_cast<const uint8_t *>(cstr), strlen(cstr));
        }

        bool equals(const Symbol *rhs) const;

        bool equals(const std::shared_ptr<Symbol> &rhs) const {
            return equals(rhs.get());
        }

        bool equals(const uint8_t *bytes, size_t len) {
            return memcmp(this->bytes, bytes, len) == 0;
        }

        bool equals(const char *cstr) {
            if(strlen(cstr) != len) {
                return false;
            }
            return memcmp(this->bytes, cstr, len) == 0;
        }

        bool operator==(const Symbol &rhs) const;

        bool operator!=(const Symbol &rhs) const;

        Hash hash();

        static Hash bytesHash(const uint8_t *bytes, int len);

    private:
        uint8_t *bytes;
        size_t len;
        Hash hashValue;
    };

    using SymbolPtr = std::shared_ptr<Symbol>;
}
