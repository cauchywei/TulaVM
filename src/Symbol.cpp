#include "Symbol.hpp"

namespace ccw::tula {

    Symbol::~Symbol() {
        delete bytes;
    }

    Symbol::Symbol(uint8_t *bytes, uint32_t len) : bytes(bytes), len(len) {
        this->bytes = static_cast<uint8_t *>(malloc(len + 1));
        this->bytes[len] = '\0';
        memcpy(this->bytes, bytes, len);
        hashValue = bytesHash(bytes, len);
    }

    bool Symbol::equals(const Symbol *rhs) const {
        return *this == *rhs;
    }

    bool Symbol::operator==(const Symbol &rhs) const {
        if (len != rhs.len) {
            return false;
        }
        if (hashValue != rhs.hashValue) {
            return false;
        }
        return memcmp(bytes, rhs.bytes, len) == 0;
    }

    bool Symbol::operator!=(const Symbol &rhs) const {
        return !(rhs == *this);
    }

    Symbol::Hash Symbol::hash() {
        return hashValue;
    }

#define A 54059u /* a prime */
#define B 76963u /* another prime */
#define C 86969u /* yet another prime */
#define FIRSTH 37u /* also prime */

    Symbol::Hash Symbol::bytesHash(const uint8_t *bytes, int len) {
        Symbol::Hash h = FIRSTH;
        for (int i = 0; i < len; ++i) {
            h = (h * A) ^ (bytes[i] * B);
        }
        return h;
    }



}