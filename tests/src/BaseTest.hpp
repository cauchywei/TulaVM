#pragma once

#include <gtest/gtest.h>

namespace CCW::Tula {
    class BaseTest : public ::testing::Test {
    protected:

    };

    class VMTest : public BaseTest {
    protected:
        void SetUp() override {
            BaseTest::SetUp();  // Sets up the base fixture first.
            
        }

        void TearDown() override {
            // clean-up work for FooTest ...
            BaseTest::TearDown();  // Remember to tear down the base fixture
        }

    };
}


