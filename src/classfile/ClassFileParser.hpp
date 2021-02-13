#pragma once

#include "ClassFileReader.hpp"

#include "../Klass.hpp"
#include "../ConstantPool.hpp"

#include <vector>

namespace CCW::Tula {

    class ClassFileParser {
    public:
        static Klass::Ptr parse(const uint8_t *data, uint32_t len) noexcept(false);

        ClassFileParser(const uint8_t *data, uint32_t len);

        Klass::Ptr parse() noexcept(false);

    private:
        static void throwParseException(const char *fmt, ...);

        void parseConstantPool() noexcept(false);

        void parseConstantPoolEntities() noexcept(false);

        void parseInterfaces() noexcept(false);

        void parseFields() noexcept(false);

        void parseAnnotations() noexcept(false);

        void parseAnnotation() noexcept(false);

        void parseTypeAnnotations() noexcept(false);

        void parseElementValue() noexcept(false);

        bool isValidCpIndex(uint16_t index);

        void parseFieldAttributes(FieldAccessFlags flags);

        const SymbolPtr& parseSignatureAttribute(uint32_t len);

    private:
        ClassFileReader reader;
        std::shared_ptr<ConstantPool> cp;

        uint16_t minorVersion{};
        uint16_t majorVersion{};
        ClassAccessFlags accessFlags {};

        std::vector<uint16_t> interfaces {};
        std::vector<uint16_t> fields {};
    };
}