#include "ConstantPool.hpp"

#include <atomic>

namespace CCW::Tula {

    struct ClassEntityInternal : public Noncopyable {
        std::atomic_intptr_t ptr;
    };

    ConstantPool::ConstantPool(uint16_t size) : size(size) {
        tags = new std::atomic<ConstantType>[size];
        entities = new intptr_t[size];
    }

    ConstantPool::~ConstantPool() {
        delete[] entities;
        delete[] tags;
    }

    void ConstantPool::putTagAt(uint16_t index, ConstantType tag) {
        CCW_ASSERT(isValidIndex(index));
        tags[index].store(tag, std::memory_order::memory_order_release);
    }

    void ConstantPool::putTagAtRelease(uint16_t index, ConstantType tag) {
        CCW_ASSERT(isValidIndex(index));
        tags[index].store(tag, std::memory_order::memory_order_release);
    }

    void ConstantPool::putClassIndexAt(uint16_t index, uint16_t nameIndex) {
        putTagAt(index, ConstantType::ClassIndex);
        entities[index] = nameIndex;
    }

    uint16_t ConstantPool::getClassIndexAt(uint16_t index) {
        auto tag = getTagAt(index);
        CCW_ASSERT(tag == ConstantType::ClassIndex);
        return entities[index];
    }

    void ConstantPool::putFieldRefAt(uint16_t index, uint16_t classIndex, uint16_t nameAndTypeIndex) {
        putTagAt(index, ConstantType::Fieldref);
        entities[index] = static_cast<uint32_t>(((uint32_t) nameAndTypeIndex) << 16u | classIndex);
    }

    uint16_t ConstantPool::getRefClassIndexAt(uint16_t index) {
        CCW_ASSERT(getTagAt(index).isReference());
        return entities[index];
    }

    uint16_t ConstantPool::getRefNameAndTypeIndexAt(uint16_t index) {
        CCW_ASSERT(getTagAt(index).isReference());
        return ((uint32_t) entities[index]) >> 16u;
    }

    void ConstantPool::putMethodRefAt(uint16_t index, uint16_t classIndex, uint16_t nameAndTypeIndex) {
        putTagAt(index, ConstantType::Methodref);
        entities[index] = static_cast<uint32_t>(((uint32_t) nameAndTypeIndex) << 16u | classIndex);
    }

    void ConstantPool::putInterfaceMethodRefAt(uint16_t index, uint16_t classIndex, uint16_t nameAndTypeIndex) {
        putTagAt(index, ConstantType::InterfaceMethodref);
        entities[index] = static_cast<uint32_t>(((uint32_t) nameAndTypeIndex) << 16u | classIndex);
    }

    void ConstantPool::putStringIndexAt(uint16_t index, uint16_t nameIndex) {
        putTagAt(index, ConstantType::StringIndex);
        entities[index] = nameIndex;
    }

    uint16_t ConstantPool::getStringIndexAt(uint16_t index) {
        CCW_ASSERT(getTagAt(index) == ConstantType::StringIndex);
        return entities[index];
    }

    void ConstantPool::putStringAt(uint16_t index, const Symbol::Ptr &symbol) {
        putTagAt(index, ConstantType::String);
        entities[index] = reinterpret_cast<intptr_t>(new Symbol::Ptr(symbol));
    }

    void ConstantPool::putIntegerAt(uint16_t index, jint value) {
        putTagAt(index, ConstantType::Integer);
        entities[index] = value;
    }

    int32_t ConstantPool::getIntegerAt(uint16_t index) {
        CCW_ASSERT(getTagAt(index) == ConstantType::Integer);
        return entities[index];
    }

    void ConstantPool::putFloatAt(uint16_t index, jfloat value) {
        putTagAt(index, ConstantType::Float);
        entities[index] = *reinterpret_cast<intptr_t *>(&value);

    }

    jfloat ConstantPool::getFloatAt(uint16_t index) {
        CCW_ASSERT(getTagAt(index) == ConstantType::Float);
        return *reinterpret_cast<jfloat *>(entities + index);
    }

    void ConstantPool::putLongAt(uint16_t index, jlong value) {
        putTagAt(index, ConstantType::Long);
        auto *p = (jlong *) (entities + index);
        *p = value;
    }

    jlong ConstantPool::getLongAt(uint16_t index) {
        CCW_ASSERT(getTagAt(index) == ConstantType::Long);
        return *(jlong *) (entities + index);
    }

    void ConstantPool::putDoubleAt(uint16_t index, jdouble value) {
        putTagAt(index, ConstantType::Double);
        auto *p = (jdouble *) (entities + index);
        *p = value;
    }

    jdouble ConstantPool::getDoubleAt(uint16_t index) {
        CCW_ASSERT(getTagAt(index) == ConstantType::Double);
        return *(jdouble *) (entities + index);
    }

    void ConstantPool::putNameAndTypeAt(uint16_t index, uint16_t nameIndex, uint16_t descriptor) {
        putTagAt(index, ConstantType::NameAndType);
        entities[index] = ((uint32_t) nameIndex) << 16u | (uint32_t) descriptor;
    }

    uint16_t ConstantPool::getNameAndTypeNameIndexAt(uint16_t index) {
        CCW_ASSERT(getTagAt(index) == ConstantType::NameAndType);
        return ((uint32_t) entities[index]) >> 16u;
    }

    uint16_t ConstantPool::getNameAndTypeDescriptorIndexAt(uint16_t index) {
        CCW_ASSERT(getTagAt(index) == ConstantType::NameAndType);
        return entities[index];
    }

    void ConstantPool::putSymbolAt(uint16_t index, const Symbol::Ptr &symbol) {
        putTagAt(index, ConstantType::Utf8);
        entities[index] = reinterpret_cast<intptr_t>(new Symbol::Ptr(symbol));
    }

    const Symbol::Ptr &ConstantPool::getSymbolAt(uint16_t index) {
        auto tag = getTagAt(index);
        CCW_ASSERT(tag == ConstantType::Utf8);
        return *((Symbol::Ptr *) entities[index]);
    }

    void ConstantPool::putMethodHandleAt(uint16_t index, uint8_t referenceKind, uint16_t referenceIndex) {
        putTagAt(index, ConstantType::MethodHandle);
        entities[index] = ((uint32_t) referenceKind) << 16u | (uint32_t) referenceIndex;
    }

    uint8_t ConstantPool::getMethodHandleReferenceKindAt(uint16_t index) {
        CCW_ASSERT(getTagAt(index) == ConstantType::MethodHandle);
        return ((uint32_t) entities[index]) >> 16u;
    }

    uint16_t ConstantPool::getMethodHandleReferenceIndexAt(uint16_t index) {
        CCW_ASSERT(getTagAt(index) == ConstantType::MethodHandle);
        return entities[index];
    }


    void ConstantPool::putMethodTypeAt(uint16_t index, uint16_t descriptorIndex) {
        putTagAt(index, ConstantType::MethodType);
        entities[index] = descriptorIndex;
    }

    uint16_t ConstantPool::getMethodTypeDescriptorIndex(uint16_t index) {
        CCW_ASSERT(getTagAt(index) == ConstantType::MethodType);
        return entities[index];
    }

    void
    ConstantPool::putInvokeDynamicAt(uint16_t index, uint16_t bootstrapMethodAttrIndex, uint16_t nameAndTypeIndex) {
        putTagAt(index, ConstantType::InvokeDynamic);
        entities[index] = ((uint32_t) bootstrapMethodAttrIndex) << 16u | (uint32_t) nameAndTypeIndex;
    }

    uint16_t ConstantPool::getInvokeDynamicBootstrapMethodAttrIndexAt(uint16_t index) {
        CCW_ASSERT(getTagAt(index) == ConstantType::InvokeDynamic);
        return ((uint32_t) entities[index]) >> 16u;
    }

    uint16_t ConstantPool::getInvokeDynamicNameAndTypeIndexAt(uint16_t index) {
        CCW_ASSERT(getTagAt(index) == ConstantType::InvokeDynamic);
        return entities[index];
    }

    void ConstantPool::putUnresolvedClassAt(uint16_t index, const Symbol::Ptr &className) {
        putTagAtRelease(index, ConstantType::UnresolvedClass);
        auto symbol = new Symbol::Ptr(className);
        auto *p = new ClassEntityInternal{};
        p->ptr.store((intptr_t) symbol, std::memory_order_release);
        entities[index] = reinterpret_cast<intptr_t>(p);
    }

    ClassEntity ConstantPool::getClassAt(uint16_t index) {
        auto tag = getTagAt(index);
        CCW_ASSERT(tag.isClassOrUnresolvedClass());
        auto *p = reinterpret_cast<ClassEntityInternal *>(entities[index]);
        if (tag == ConstantType::UnresolvedClass) {
            intptr_t ptr = p->ptr.load(std::memory_order_acquire);
            return ClassEntity(*((Symbol::Ptr *) ptr));
        } else {
            // TODO return class
            intptr_t ptr = p->ptr.load(std::memory_order_acquire);
            return ClassEntity(*((Symbol::Ptr *) ptr));
        }
    }

}