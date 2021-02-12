#include "ClassFileReader.hpp"

namespace CCW::Tula {

    u8 ClassFileReader::readU8() {
        ensure(1, "buffer overflow");
        return 0;
    }

    u16 ClassFileReader::readU16() {
        ensure(2, "buffer overflow");
        return readU16Unchecked();
    }

    u32 ClassFileReader::readU32() {
        ensure(4, "buffer overflow");
        return readU32Unchecked();
    }

    u64 ClassFileReader::readU64() {
        ensure(8, "buffer overflow");
        return readU64Unchecked();
    }

    void ClassFileReader::skip(uint32_t size) {
        ensure(size, "buffer overflow");
        return skipUnchecked(size);
    }
}