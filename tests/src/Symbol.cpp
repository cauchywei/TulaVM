#include <gtest/gtest.h>
#include <cstring>

#include "Symbol.hpp"

using namespace ccw::tula;

TEST(Symbol, TestSymbolEquals) {
    const char *a_bytes = "abcde";
    size_t a_len = strlen(a_bytes);
    auto symbol_a = Symbol::create((uint8_t *) a_bytes, a_len);
    auto symbol_a_copy = Symbol::create((uint8_t *) a_bytes, a_len);
    ASSERT_EQ(symbol_a->hash(), symbol_a_copy->hash());
    ASSERT_TRUE(symbol_a->equals(symbol_a_copy));

    const char *b_bytes = "cccdddd";
    size_t b_len = strlen(b_bytes);
    auto symbol_b = Symbol::create((uint8_t *) b_bytes, b_len);
    ASSERT_NE(symbol_a->hash(), symbol_b->hash());
    ASSERT_FALSE(symbol_a->equals(symbol_b));
}