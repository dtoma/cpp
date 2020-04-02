#include <algorithm>

#include "gtest/gtest.h"

#include "string.hpp"

using namespace dtoma;

TEST(TestString, TestDefaultConstructor)
{
    String s;
    ASSERT_TRUE(s.sso());
    ASSERT_EQ(s.size(), 0);
    ASSERT_EQ(s.capacity(), 23);
}

TEST(TestString, TestSmallString)
{
    char str[] = "Hello, World!";
    String s(str);
    ASSERT_TRUE(s.sso());
    ASSERT_STREQ(s.data(), str);
}

TEST(TestString, TestLongString)
{
    char str[] = "This should be a sufficiently long string";
    String s(str);
    ASSERT_FALSE(s.sso());
    ASSERT_STREQ(s.data(), str);
}

TEST(TestString, TestIterator)
{
    String s("Hello, World!");
    std::transform(
        std::begin(s),
        std::end(s),
        std::begin(s),
        [](auto const &c) -> char {
            return std::toupper(c);
        });
    ASSERT_STREQ(s.data(), "HELLO, WORLD!");
}

TEST(TestString, TestReverseIterator)
{
    String s("Hello, World!");
    std::string out;
    std::for_each(
        std::rbegin(s),
        std::rend(s),
        [&](auto const &c) {
            out += c;
        });
    ASSERT_STREQ(out.c_str(), "!dlroW ,olleH");
}