#include <gtest/gtest.h>
#include "../AdaptiveSequence.h"

TEST(AdaptiveSequence, StartsAsArray) {
    AdaptiveSequence<int> s;
    EXPECT_TRUE(s.IsArray());
    EXPECT_EQ(s.GetLength(), 0);
}

TEST(AdaptiveSequence, Append) {
    AdaptiveSequence<int> s; s.Append(1); s.Append(2); s.Append(3);
    EXPECT_EQ(s.GetLength(), 3);
    EXPECT_EQ(s.Get(0), 1);
    EXPECT_EQ(s.Get(2), 3);
}

TEST(AdaptiveSequence, Prepend) {
    AdaptiveSequence<int> s; s.Append(2); s.Prepend(1);
    EXPECT_EQ(s.GetFirst(), 1);
}

TEST(AdaptiveSequence, InsertAt) {
    AdaptiveSequence<int> s; s.Append(1); s.Append(3); s.InsertAt(2, 1);
    EXPECT_EQ(s.Get(1), 2);
    EXPECT_EQ(s.GetLength(), 3);
}

TEST(AdaptiveSequence, GetFirstGetLast) {
    AdaptiveSequence<int> s; s.Append(10); s.Append(20);
    EXPECT_EQ(s.GetFirst(), 10);
    EXPECT_EQ(s.GetLast(), 20);
}

TEST(AdaptiveSequence, Indexing) {
    AdaptiveSequence<int> s; s.Append(5); s.Append(15);
    EXPECT_EQ(s[0], 5);
    EXPECT_EQ(s[1], 15);
}

TEST(AdaptiveSequence, ManyInsertAtSwitchesToList) {
    AdaptiveSequence<int> s;
    for (int i = 0; i < 5; ++i) s.Append(i);
    for (int i = 0; i < 15; ++i) s.InsertAt(99, 0);
    EXPECT_FALSE(s.IsArray());
}

TEST(AdaptiveSequence, ManyGetSwitchesBackToArray) {
    AdaptiveSequence<int> s;
    for (int i = 0; i < 5; ++i) s.Append(i);
    for (int i = 0; i < 15; ++i) s.InsertAt(0, 0);
    EXPECT_FALSE(s.IsArray());
    for (int i = 0; i < 25; ++i) s.Get(0);
    EXPECT_TRUE(s.IsArray());
}

TEST(AdaptiveSequence, DataPreservedAfterSwitch) {
    AdaptiveSequence<int> s; s.Append(1); s.Append(2); s.Append(3);
    for (int i = 0; i < 15; ++i) s.InsertAt(0, 0);
    EXPECT_EQ(s.Get(s.GetLength() - 3), 1);
    EXPECT_EQ(s.Get(s.GetLength() - 2), 2);
    EXPECT_EQ(s.Get(s.GetLength() - 1), 3);
}

TEST(AdaptiveSequence, SingleElement) {
    AdaptiveSequence<int> s; s.Append(42);
    EXPECT_EQ(s.GetFirst(), 42);
    EXPECT_EQ(s.GetLast(), 42);
    EXPECT_EQ(s.GetLength(), 1);
}

TEST(AdaptiveSequence, GetSubsequence) {
    AdaptiveSequence<int> s;
    for (int i = 0; i < 5; ++i) s.Append(i);
    auto* sub = s.GetSubsequence(1, 3);
    EXPECT_EQ(sub->GetLength(), 3);
    EXPECT_EQ(sub->Get(0), 1);
    delete sub;
}

TEST(AdaptiveSequence, GetFirstOnEmptyThrows) {
    AdaptiveSequence<int> s;
    EXPECT_ANY_THROW(s.GetFirst());
}

TEST(AdaptiveSequence, GetLastOnEmptyThrows) {
    AdaptiveSequence<int> s;
    EXPECT_ANY_THROW(s.GetLast());
}

TEST(AdaptiveSequence, GetOutOfRange) {
    AdaptiveSequence<int> s; s.Append(1);
    EXPECT_ANY_THROW(s.Get(5));
    EXPECT_ANY_THROW(s.Get(-1));
}
