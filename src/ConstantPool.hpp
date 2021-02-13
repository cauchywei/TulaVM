#pragma once

#include "JVM.hpp"
#include "Symbol.hpp"

#include <cstdint>
#include <utility>
#include <variant>

namespace CCW::Tula {

    struct ConstantTag {
        ConstantType type;

        [[nodiscard]] inline bool isReference() const {
            return type == ConstantType::Fieldref
                   || type == ConstantType::Methodref
                   || type == ConstantType::InterfaceMethodref;
        }

        [[nodiscard]] inline bool isClassOrIndex() const {
            return isClassOrUnresolvedClass() || type == ConstantType::ClassIndex;
        }

        [[nodiscard]] inline bool isClass() const {
            return type == ConstantType::Class;
        }

        [[nodiscard]] inline bool isUnresolvedClass() const {
            return type == ConstantType::UnresolvedClass;
        }

        [[nodiscard]] inline bool isClassOrUnresolvedClass() const {
            return type == ConstantType::Class
                   || type == ConstantType::UnresolvedClass;
        }

        [[nodiscard]] inline bool isUtf8() const {
            return type == ConstantType::Utf8;
        }

        [[nodiscard]] inline bool isInteger() const {
            return type == ConstantType ::Integer;
        }

        [[nodiscard]] inline bool isLong() const {
            return type == ConstantType ::Long;
        }

        [[nodiscard]] inline bool isDouble() const {
            return type == ConstantType ::Double;
        }

        [[nodiscard]] inline bool isFloat() const {
            return type == ConstantType ::Float;
        }

        [[nodiscard]] inline bool isConstantValueType() const {
            return type == ConstantType::Long
                   || type == ConstantType::Float
                   || type == ConstantType::Double
                   || type == ConstantType::Integer
                   || type == ConstantType::String;
        }

        bool operator==(const ConstantTag &rhs) const {
            return type == rhs.type;
        }

        bool operator==(const ConstantType &rhstype) const {
            return type == rhstype;
        }

        bool operator!=(const ConstantTag &rhs) const {
            return !(rhs == *this);
        }
    };


    class ClassEntity {
    public:
        explicit ClassEntity(SymbolPtr unresolvedClassName) : unresolvedClassName(std::move(unresolvedClassName)) {}

        bool isUnresolved() {
            return unresolvedClassName != nullptr;
        }

        const SymbolPtr &getUnresolvedClassName() {
            CCW_ASSERT(isUnresolved());
            return unresolvedClassName;
        }

    private:
        SymbolPtr unresolvedClassName;
    };


    class ConstantPool : public Noncopyable {
    public:
        explicit ConstantPool(uint16_t size);

        [[nodiscard]] inline bool isValidIndex(uint16_t index) const {
            return index > 0 && index <= size;
        }

        [[nodiscard]] uint16_t getSize() const {
            return size;
        }

        void putTagAt(uint16_t index, ConstantType tag);

        void putTagAtRelease(uint16_t index, ConstantType tag);

        ConstantTag getTagAt(uint16_t index) {
            CCW_ASSERT(isValidIndex(index));
            return ConstantTag{tags[index].load(std::memory_order_relaxed)};
        }

        ConstantType getConstantTypeAt(uint16_t index) {
            CCW_ASSERT(isValidIndex(index));
            return tags[index].load(std::memory_order_relaxed);
        }

        void putClassIndexAt(uint16_t index, uint16_t nameIndex);

        uint16_t getClassIndexAt(uint16_t index);

        void putFieldRefAt(uint16_t index, uint16_t classIndex, uint16_t nameAndTypeIndex);

        uint16_t getRefClassIndexAt(uint16_t index);

        uint16_t getRefNameAndTypeIndexAt(uint16_t index);

        void putMethodRefAt(uint16_t index, uint16_t classIndex, uint16_t nameAndTypeIndex);

        void putInterfaceMethodRefAt(uint16_t index, uint16_t classIndex, uint16_t nameAndTypeIndex);

        void putStringIndexAt(uint16_t index, uint16_t nameIndex);

        uint16_t getStringIndexAt(uint16_t index);

        void putStringAt(uint16_t index, const SymbolPtr &symbol);

        void putIntegerAt(uint16_t index, int32_t value);

        int32_t getIntegerAt(uint16_t index);

        void putFloatAt(uint16_t index, jfloat value);

        jfloat getFloatAt(uint16_t index);

        void putLongAt(uint16_t index, jlong value);

        jlong getLongAt(uint16_t index);

        void putDoubleAt(uint16_t index, jdouble value);

        jdouble getDoubleAt(uint16_t index);

        void putNameAndTypeAt(uint16_t index, uint16_t nameIndex, uint16_t descriptor);

        uint16_t getNameAndTypeNameIndexAt(uint16_t index);

        uint16_t getNameAndTypeDescriptorIndexAt(uint16_t index);

        void putSymbolAt(uint16_t index, const SymbolPtr &symbol);

        const SymbolPtr &getSymbolAt(uint16_t index);

        void putMethodHandleAt(uint16_t index, uint8_t referenceKind, uint16_t referenceIndex);

        uint8_t getMethodHandleReferenceKindAt(uint16_t index);

        uint16_t getMethodHandleReferenceIndexAt(uint16_t index);

        void putMethodTypeAt(uint16_t index, uint16_t descriptorIndex);

        uint16_t getMethodTypeDescriptorIndex(uint16_t index);

        void putInvokeDynamicAt(uint16_t index, uint16_t bootstrapMethodAttrIndex, uint16_t nameAndTypeIndex);

        uint16_t getInvokeDynamicBootstrapMethodAttrIndexAt(uint16_t index);

        uint16_t getInvokeDynamicNameAndTypeIndexAt(uint16_t index);

        void putUnresolvedClassAt(uint16_t index, const SymbolPtr &className);

        virtual ~ConstantPool();

        ClassEntity getClassAt(uint16_t index);

    private:
        uint16_t size;
        std::atomic<ConstantType> *tags;
        intptr_t *entities;
    };
}

