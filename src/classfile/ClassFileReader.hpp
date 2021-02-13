#pragma once

#include "../Types.hpp"
#include "../Error.hpp"

#include <CCW/Base.hpp>

namespace CCW::Tula {
    class ClassFileReader : public Noncopyable {
    public:

        ClassFileReader(const uint8_t *bytes, uint32_t len) :
                bytes(bytes),
                len(len),
                ptr(nullptr),
                end(nullptr) {
            ptr = const_cast<uint8_t *>(this->bytes);
            end = const_cast<uint8_t *>(bytes + len);
        }

        inline void ensure(uint32_t least, const std::string &msg = "Truncated class file") noexcept(false) {
            if (remaining() < least) {
                throw ClassFormatError(msg);
            }
        }

        inline uint8_t* buffer() {
            return ptr;
        }


        void skip(uint32_t size);
        inline void skipUnchecked(uint32_t size) {
            ptr += size;
        }

        uint8_t readU8() noexcept(false);

        inline uint8_t readU8Unchecked() noexcept {
            return *ptr++;
        }

        uint16_t readU16() noexcept(false);

        inline uint16_t readU16Unchecked() noexcept {
            auto a = uint16_t(ptr[0]) << 8 | uint16_t(ptr[1]);
            ptr += 2;
            return a;
        }

        uint32_t readU32() noexcept(false);

        inline uint32_t readU32Unchecked() noexcept {
            auto a = uint32_t(ptr[0]) << 24 |
                     uint32_t(ptr[1]) << 16 |
                     uint32_t(ptr[2]) << 8 |
                     uint32_t(ptr[3]);
            ptr += 4;
            return a;
        }

        uint64_t readU64() noexcept(false);

        inline uint64_t readU64Unchecked() noexcept {
            auto a = uint64_t(ptr[0]) << 56 |
                     uint64_t(ptr[1]) << 48 |
                     uint64_t(ptr[2]) << 40 |
                     uint64_t(ptr[3]) << 32 |
                     uint64_t(ptr[4]) << 24 |
                     uint64_t(ptr[5]) << 16 |
                     uint64_t(ptr[6]) << 8 |
                     uint64_t(ptr[7]);
            ptr += 8;
            return a;
        }

        inline bool isEos() {
            return remaining() == 0;
        }

    private:
        inline uint32_t remaining() {
            return end - ptr;
        }

    private:
        const uint8_t *bytes;
        const uint32_t len;
        uint8_t *ptr;
        const uint8_t *end;
    };
}
