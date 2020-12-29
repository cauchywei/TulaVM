#pragma once

#include <ccw/Base.hpp>
#include <memory>

namespace ccw::tula {
    class Symbol : public Noncopyable, public std::enable_shared_from_this<Symbol> {
    public:
        using Hash = uint32_t;
        using Ptr = std::shared_ptr<Symbol>;

        Symbol(uint8_t *bytes, uint32_t len);

        virtual ~Symbol();

        static inline Symbol::Ptr create(uint8_t *bytes, int len) {
            return std::make_shared<Symbol>(bytes, len);
        }

        bool equals(const Symbol *rhs) const;

        bool equals(const Symbol::Ptr &rhs) const {
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
        uint32_t len;
        Hash hashValue;
    };

}
