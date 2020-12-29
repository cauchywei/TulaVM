#include <gtest/gtest.h>
#include <tula/VM.hpp>

using namespace std;
using namespace ccw::tula;

TEST(TestVM, TestSimpleClass) {
    ASSERT_TRUE(VM::current() == nullptr);
    {
        unique_ptr<VM> pVm = make_unique<VM>("", "data/test/tula/Test.class");
        ASSERT_TRUE(VM::current() == pVm.get());
        pVm->start();
    }
    ASSERT_TRUE(VM::current() == nullptr);
}