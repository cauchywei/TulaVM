#pragma once

#include "../Types.hpp"
#include "../Error.hpp"

#include <CCW/Base.hpp>

namespace CCW::Tula {
    class ClassFileReader : public Noncopyable {
    public:

        ClassFileReader(const u8 *bytes, uint32_t len) :
                bytes(bytes),
                len(len),
                ptr(nullptr),
                end(nullptr) {
            ptr = const_cast<u8 *>(this->bytes);
            end = const_cast<u8 *>(bytes + len);
        }

        inline void ensure(uint32_t least, const std::string &msg = "Truncated class file") noexcept(false) {
            if (remaining() < least) {
                throw ClassFormatError(msg);
            }
        }

        inline u8* buffer() {
            return ptr;
        }


        void skip(uint32_t size);
        inline void skipUnchecked(uint32_t size) {
            ptr += size;
        }

        u8 readU8() noexcept(false);

        inline u8 readU8Unchecked() noexcept {
            return *ptr++;
        }

        u16 readU16() noexcept(false);

        inline u16 readU16Unchecked() noexcept {
            auto a = u16(ptr[0]) << 8 | u16(ptr[1]);
            ptr += 2;
            return a;
        }

        u32 readU32() noexcept(false);

        inline u32 readU32Unchecked() noexcept {
            auto a = u32(ptr[0]) << 24 |
                     u32(ptr[1]) << 16 |
                     u32(ptr[2]) << 8 |
                     u32(ptr[3]);
            ptr += 4;
            return a;
        }

        u64 readU64() noexcept(false);

        inline u64 readU64Unchecked() noexcept {
            auto a = u64(ptr[0]) << 56 |
                     u64(ptr[1]) << 48 |
                     u64(ptr[2]) << 40 |
                     u64(ptr[3]) << 32 |
                     u64(ptr[4]) << 24 |
                     u64(ptr[5]) << 16 |
                     u64(ptr[6]) << 8 |
                     u64(ptr[7]);
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
        const u8 *bytes;
        const uint32_t len;
        u8 *ptr;
        const u8 *end;
    };
}
