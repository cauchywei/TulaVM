#include <gtest/gtest.h>
#include <cstring>

#include "Symbol.hpp"

using namespace ccw::tula;

TEST(Symbol, TestSymbolEquals) {
    const char *a_bytes = "abcde";
    auto symbol_a = Symbol::create(a_bytes);
    auto symbol_a_copy = Symbol::create(a_bytes);
    ASSERT_EQ(symbol_a->hash(), symbol_a_copy->hash());
    ASSERT_TRUE(symbol_a->equals(symbol_a_copy));

    const char *b_bytes = "cccdddd";
    auto symbol_b = Symbol::create(b_bytes);
    ASSERT_NE(symbol_a->hash(), symbol_b->hash());
    ASSERT_FALSE(symbol_a->equals(symbol_b));
}