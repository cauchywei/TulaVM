#pragma once

#include <cstdint>
#include "utils/Enum.hpp"

namespace ccw::tula {

    typedef unsigned char jboolean;
    typedef unsigned short jchar;
    typedef short jshort;
    typedef float jfloat;
    typedef double jdouble;

    typedef int32_t jint;
    typedef jint jsize;
    typedef int64_t jlong;


    enum class ConstantType : uint8_t {
        Utf8 = 1,
        Unicode,               /* unused */
        Integer,
        Float,
        Long,
        Double,
        Class,
        String,
        Fieldref,
        Methodref,
        InterfaceMethodref,
        NameAndType,
        MethodHandle = 15,  // JSR 292
        MethodType = 16,  // JSR 292
        // (unused)             = 17,  // JSR 292 early drafts only
                InvokeDynamic = 18,  // JSR 292


        ClassIndex = 100,
        StringIndex = 101,
        UnresolvedClass = 102
    };

/* JVM_CONSTANT_MethodHandle subtypes */
    enum class ReferenceKind : uint8_t {
        REF_getField = 1,
        REF_getStatic = 2,
        REF_putField = 3,
        REF_putStatic = 4,
        REF_invokeVirtual = 5,
        REF_invokeStatic = 6,
        REF_invokeSpecial = 7,
        REF_newInvokeSpecial = 8,
        REF_invokeInterface = 9
    };

    enum class ClassAccessFlags : uint32_t {
        Public = 0x0001,
        Final = 0x0010,
        Super = 0x0020,
        Interface = 0x0200,
        Abstract = 0x0400,
        Synthetic = 0x1000,
        Annotation = 0x2000,
        Enum = 0x4000
    };

    enum class FieldAccessFlags : uint32_t {
        Public = 0x0001,
        Private = 0x0002,
        Protected = 0x0004,
        Static = 0x0008,
        Final = 0x0010,
        Volatile = 0x0040,
        Transient = 0x0080,
        Synthetic = 0x1000,
        Enum = 0x4000
    };

    enum class ElementValueTag : char {
        Byte = 'B',
        Char = 'C',
        Double = 'D',
        Float = 'F',
        Int = 'I',
        Long = 'J',
        Short = 'S',
        Boolean = 'Z',
        String = 's',
        EnumType = 'e',
        Class = 'c',
        AnnotationType = '@',
        ArrayType = '['
    };
}

ENABLE_BITMASK_OPERATORS(ccw::tula::ClassAccessFlags);
ENABLE_BITMASK_OPERATORS(ccw::tula::FieldAccessFlags);
