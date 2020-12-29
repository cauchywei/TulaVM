#include <gtest/gtest.h>
#include <JVM.hpp>
#include <ConstantPool.hpp>

namespace ccw::tula {

    TEST(TestConstantPool, TestUtf8) {
        ConstantPool cp(10);
        ASSERT_EQ(cp.getSize(), 10);
        uint16_t index = 1;

        const char *className = "test/java/Hello";
        auto symbol = Symbol::create((uint8_t *) className, strlen(className));
        cp.putSymbolAt(index, symbol);
        const Symbol::Ptr &getSymbol = cp.getSymbolAt(index);
        ASSERT_EQ(ConstantType::Utf8, cp.getConstantTypeAt(index));
        ASSERT_TRUE(getSymbol->equals((uint8_t *) className, strlen(className)));
    }


    TEST(TestConstantPool, TestInteger) {
        ConstantPool cp(10);
        ASSERT_EQ(cp.getSize(), 10);
        uint16_t index = 1;
        int32_t max = std::numeric_limits<int32_t>::max();
        int32_t min = std::numeric_limits<int32_t>::min();
        cp.putIntegerAt(index, 0);
        ASSERT_EQ(ConstantType::Integer, cp.getConstantTypeAt(index));
        ASSERT_EQ(0, cp.getIntegerAt(index));

        cp.putIntegerAt(index, 1);
        ASSERT_EQ(ConstantType::Integer, cp.getConstantTypeAt(index));
        ASSERT_EQ(1, cp.getIntegerAt(index));

        cp.putIntegerAt(index, -1);
        ASSERT_EQ(ConstantType::Integer, cp.getConstantTypeAt(index));
        ASSERT_EQ(-1, cp.getIntegerAt(index));

        cp.putIntegerAt(index, min);
        ASSERT_EQ(ConstantType::Integer, cp.getConstantTypeAt(index));
        ASSERT_EQ(min, cp.getIntegerAt(index));

        cp.putIntegerAt(index, max);
        ASSERT_EQ(ConstantType::Integer, cp.getConstantTypeAt(index));
        ASSERT_EQ(max, cp.getIntegerAt(index));

        index++;
    }

    TEST(TestConstantPool, TestFloat) {
        ConstantPool cp(10);
        ASSERT_EQ(cp.getSize(), 10);
        uint16_t index = 1;
        auto max = std::numeric_limits<jfloat>::max();
        auto min = std::numeric_limits<jfloat>::min();
        cp.putFloatAt(index, 0);
        ASSERT_EQ(ConstantType::Float, cp.getConstantTypeAt(index));
        ASSERT_EQ(0, cp.getFloatAt(index));

        cp.putFloatAt(index, 1);
        ASSERT_EQ(ConstantType::Float, cp.getConstantTypeAt(index));
        ASSERT_EQ(1, cp.getFloatAt(index));

        cp.putFloatAt(index, -1);
        ASSERT_EQ(ConstantType::Float, cp.getConstantTypeAt(index));
        ASSERT_EQ(-1, cp.getFloatAt(index));

        cp.putFloatAt(index, min);
        ASSERT_EQ(ConstantType::Float, cp.getConstantTypeAt(index));
        ASSERT_EQ(min, cp.getFloatAt(index));

        cp.putFloatAt(index, max);
        ASSERT_EQ(ConstantType::Float, cp.getConstantTypeAt(index));
        ASSERT_EQ(max, cp.getFloatAt(index));

        index++;
    }

    TEST(TestConstantPool, TestLong) {
        ConstantPool cp(10);
        ASSERT_EQ(cp.getSize(), 10);
        uint16_t index = 1;
        auto max = std::numeric_limits<jlong>::max();
        auto min = std::numeric_limits<jlong>::min();
        cp.putLongAt(index, 0);
        ASSERT_EQ(ConstantType::Long, cp.getConstantTypeAt(index));
        ASSERT_EQ(0, cp.getLongAt(index));

        cp.putLongAt(index, 1);
        ASSERT_EQ(ConstantType::Long, cp.getConstantTypeAt(index));
        ASSERT_EQ(1, cp.getLongAt(index));

        cp.putLongAt(index, -1);
        ASSERT_EQ(ConstantType::Long, cp.getConstantTypeAt(index));
        ASSERT_EQ(-1, cp.getLongAt(index));

        cp.putLongAt(index, min);
        ASSERT_EQ(ConstantType::Long, cp.getConstantTypeAt(index));
        ASSERT_EQ(min, cp.getLongAt(index));

        cp.putLongAt(index, max);
        ASSERT_EQ(ConstantType::Long, cp.getConstantTypeAt(index));
        ASSERT_EQ(max, cp.getLongAt(index));

        index++;
        index++;
    }

    TEST(TestConstantPool, TestDouble) {
        ConstantPool cp(10);
        ASSERT_EQ(cp.getSize(), 10);
        uint16_t index = 1;
        auto max = std::numeric_limits<jdouble>::max();
        auto min = std::numeric_limits<jdouble>::min();
        cp.putDoubleAt(index, 0);
        ASSERT_EQ(ConstantType::Double, cp.getConstantTypeAt(index));
        ASSERT_EQ(0, cp.getDoubleAt(index));

        cp.putDoubleAt(index, 1);
        ASSERT_EQ(ConstantType::Double, cp.getConstantTypeAt(index));
        ASSERT_EQ(1, cp.getDoubleAt(index));

        cp.putDoubleAt(index, -1);
        ASSERT_EQ(ConstantType::Double, cp.getConstantTypeAt(index));
        ASSERT_EQ(-1, cp.getDoubleAt(index));

        cp.putDoubleAt(index, min);
        ASSERT_EQ(ConstantType::Double, cp.getConstantTypeAt(index));
        ASSERT_EQ(min, cp.getDoubleAt(index));

        cp.putDoubleAt(index, max);
        ASSERT_EQ(ConstantType::Double, cp.getConstantTypeAt(index));
        ASSERT_EQ(max, cp.getDoubleAt(index));

        index++;
        index++;
    }

    // TODO add class entity te

    TEST(TestConstantPool, TestRef) {
        ConstantPool cp(10);
        ASSERT_EQ(cp.getSize(), 10);
        uint16_t index = 1;
        cp.putFieldRefAt(index, 22, 33);
        ASSERT_EQ(ConstantType::Fieldref, cp.getConstantTypeAt(index));
        ASSERT_EQ(22, cp.getRefClassIndexAt(index));
        ASSERT_EQ(33, cp.getRefNameAndTypeIndexAt(index));
        index++;

        cp.putMethodRefAt(index, 22, 33);
        ASSERT_EQ(ConstantType::Methodref, cp.getConstantTypeAt(index));
        ASSERT_EQ(22, cp.getRefClassIndexAt(index));
        ASSERT_EQ(33, cp.getRefNameAndTypeIndexAt(index));
        index++;

        cp.putInterfaceMethodRefAt(index, 22, 33);
        ASSERT_EQ(ConstantType::InterfaceMethodref, cp.getConstantTypeAt(index));
        ASSERT_EQ(22, cp.getRefClassIndexAt(index));
        ASSERT_EQ(33, cp.getRefNameAndTypeIndexAt(index));
        index++;
    }

    TEST(TestConstantPool, TestNameAndType) {
        ConstantPool cp(10);
        ASSERT_EQ(cp.getSize(), 10);
        uint16_t index = 1;
        cp.putNameAndTypeAt(index, 22, 33);
        ASSERT_EQ(ConstantType::NameAndType, cp.getConstantTypeAt(index));
        ASSERT_EQ(22, cp.getNameAndTypeNameIndexAt(index));
        ASSERT_EQ(33, cp.getNameAndTypeDescriptorIndexAt(index));
    }

    TEST(TestConstantPool, TestMethodHandle) {
        ConstantPool cp(10);
        uint16_t index = 1;
        cp.putMethodHandleAt(index, 22, 33);
        ASSERT_EQ(ConstantType::MethodHandle, cp.getConstantTypeAt(index));
        ASSERT_EQ(22, cp.getMethodHandleReferenceKindAt(index));
        ASSERT_EQ(33, cp.getMethodHandleReferenceIndexAt(index));
        index++;
    }

    TEST(TestConstantPool, TestMethodType) {
        ConstantPool cp(10);
        ASSERT_EQ(cp.getSize(), 10);
        uint16_t index = 1;
        cp.putMethodTypeAt(index, 22);
        ASSERT_EQ(ConstantType::MethodType, cp.getConstantTypeAt(index));
        ASSERT_EQ(22, cp.getMethodTypeDescriptorIndex(index));
        index++;
    }

    TEST(TestConstantPool, TestClassIndex) {
        ConstantPool cp(10);
        ASSERT_EQ(cp.getSize(), 10);
        uint16_t index = 1;
        cp.putClassIndexAt(index, 22);
        ASSERT_EQ(ConstantType::ClassIndex, cp.getConstantTypeAt(index));
        ASSERT_EQ(22, cp.getClassIndexAt(index));
        index++;
    }

    TEST(TestConstantPool, TestStringIndex) {
        ConstantPool cp(10);
        ASSERT_EQ(cp.getSize(), 10);
        uint16_t index = 1;
        cp.putStringIndexAt(index, 22);
        ASSERT_EQ(ConstantType::StringIndex, cp.getConstantTypeAt(index));
        ASSERT_EQ(22, cp.getStringIndexAt(index));
        index++;
    }

}