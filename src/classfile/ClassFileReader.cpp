#include "ClassFileReader.hpp"

namespace CCW::Tula {

    uint8_t ClassFileReader::readU8() {
        ensure(1, "buffer overflow");
        return 0;
    }

    uint16_t ClassFileReader::readU16() {
        ensure(2, "buffer overflow");
        return readU16Unchecked();
    }

    uint32_t ClassFileReader::readU32() {
        ensure(4, "buffer overflow");
        return readU32Unchecked();
    }

    uint64_t ClassFileReader::readU64() {
        ensure(8, "buffer overflow");
        return readU64Unchecked();
    }

    void ClassFileReader::skip(uint32_t size) {
        ensure(size, "buffer overflow");
        return skipUnchecked(size);
    }
}