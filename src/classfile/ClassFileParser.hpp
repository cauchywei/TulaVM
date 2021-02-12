#pragma once

#include "ClassFileReader.hpp"

#include "../Klass.hpp"
#include "../ConstantPool.hpp"

using namespace std;

namespace CCW::Tula {

    class ClassFileParser {
    public:
        static Klass::Ptr parse(const u8 *data, uint32_t len) noexcept(false);

        ClassFileParser(const u8 *data, uint32_t len);

        Klass::Ptr parse() noexcept(false);


    private:
        ClassFileReader reader;
        shared_ptr<ConstantPool> cp;

        u16 minorVersion{};
        u16 majorVersion{};
        ClassAccessFlags accessFlags;
    private:
        static void throwParseException(const char *fmt, ...);

        shared_ptr<ConstantPool> parseConstantPool();

        void parseConstantPoolEntities();

        vector<uint16_t> parseInterfaces(uint16_t size);



        bool isValidCpIndex(uint16_t index);

        void parseFields();

        void parseFieldAttributes(FieldAccessFlags flags);

        const Symbol::Ptr & parseSignatureAttribute(u32 len);
    };
}