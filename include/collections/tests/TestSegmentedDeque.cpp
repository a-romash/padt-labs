#include <gtest/gtest.h>
#include <sstream>
#include "../SegmentedDeque.h"
#include "../SegmentedDequeIO.h"

TEST(SegmentedDeque, EmptyLengthZero) {
    SegmentedDeque<int> d(4);
    EXPECT_EQ(d.GetLength(), 0);
    EXPECT_TRUE(d.IsEmpty());
}

TEST(SegmentedDeque, Append) {
    SegmentedDeque<int> d(4);
    for (int i = 0; i < 5; ++i) d.Append(i);
    EXPECT_EQ(d.GetLength(), 5);
    EXPECT_EQ(d.GetFirst(), 0);
    EXPECT_EQ(d.GetLast(), 4);
    EXPECT_EQ(d.Get(2), 2);
}

TEST(SegmentedDeque, Prepend) {
    SegmentedDeque<int> d(4);
    for (int i = 1; i <= 5; ++i) d.Prepend(i);   // 5,4,3,2,1
    EXPECT_EQ(d.GetLength(), 5);
    EXPECT_EQ(d.GetFirst(), 5);
    EXPECT_EQ(d.GetLast(), 1);
    EXPECT_EQ(d.Get(0), 5);
}

TEST(SegmentedDeque, GetAcrossSegments) {
    SegmentedDeque<int> d(3);
    for (int i = 0; i < 6; ++i) d.Append(i);
    bool across = true;
    for (int i = 0; i < 6; ++i) if (d.Get(i) != i) across = false;
    EXPECT_TRUE(across);
}

TEST(SegmentedDeque, AppendPrependReturnThis) {
    SegmentedDeque<int> d(4);
    Sequence<int>* r1 = d.Append(1);
    EXPECT_EQ(r1, &d);
    Sequence<int>* r2 = d.Prepend(0);
    EXPECT_EQ(r2, &d);
    EXPECT_EQ(d.GetFirst(), 0);
    EXPECT_EQ(d.GetLast(), 1);
}

// Регрессия на баг #1: InsertAt мутирует дек на месте и возвращает this
TEST(SegmentedDeque, InsertAtMutatesInPlace) {
    SegmentedDeque<int> d(4);
    for (int i = 0; i < 4; ++i) d.Append(i);   // 0,1,2,3
    Sequence<int>* r = d.InsertAt(99, 2);
    EXPECT_EQ(r, &d);
    EXPECT_EQ(d.GetLength(), 5);
    EXPECT_EQ(d.Get(2), 99);
    EXPECT_EQ(d.Get(3), 2);
}

TEST(SegmentedDeque, InsertAtEnds) {
    SegmentedDeque<int> d(4);
    for (int i = 0; i < 3; ++i) d.Append(i);   // 0,1,2
    d.InsertAt(7, 0);
    EXPECT_EQ(d.GetFirst(), 7);
    d.InsertAt(8, d.GetLength());
    EXPECT_EQ(d.GetLast(), 8);
}

TEST(SegmentedDeque, Pop) {
    SegmentedDeque<int> d(4);
    for (int i = 0; i < 5; ++i) d.Append(i);   // 0..4
    EXPECT_EQ(d.PopFirst(), 0);
    EXPECT_EQ(d.PopLast(), 4);
    EXPECT_EQ(d.GetLength(), 3);
    EXPECT_EQ(d.GetFirst(), 1);
    EXPECT_EQ(d.GetLast(), 3);
}

TEST(SegmentedDeque, DrainThenReuse) {
    SegmentedDeque<int> d(4);
    d.Append(1); d.Append(2);
    d.PopFirst(); d.PopLast();
    EXPECT_TRUE(d.IsEmpty());
    d.Append(42);
    EXPECT_EQ(d.GetFirst(), 42);
    EXPECT_EQ(d.GetLast(), 42);
}

TEST(SegmentedDeque, GetSubsequence) {
    SegmentedDeque<int> d(4);
    for (int i = 0; i < 5; ++i) d.Append(i);   // 0..4
    Sequence<int>* sub = d.GetSubsequence(1, 3);
    EXPECT_EQ(sub->GetLength(), 3);
    EXPECT_EQ(sub->Get(0), 1);
    EXPECT_EQ(sub->Get(2), 3);
    EXPECT_EQ(d.GetLength(), 5);
    delete sub;
}

TEST(SegmentedDeque, Concat) {
    SegmentedDeque<int> a(4), b(4);
    a.Append(1); a.Append(2);
    b.Append(3); b.Append(4);
    Sequence<int>* r = a.Concat(b);
    EXPECT_EQ(r->GetLength(), 4);
    EXPECT_EQ(r->Get(2), 3);
    EXPECT_EQ(a.GetLength(), 2);
    delete r;
}

TEST(SegmentedDeque, Indexing) {
    SegmentedDeque<int> d(4);
    for (int i = 0; i < 3; ++i) d.Append(i * 10);
    EXPECT_EQ(d[0], 0);
    EXPECT_EQ(d[2], 20);
}

TEST(SegmentedDeque, FindSubsequence) {
    SegmentedDeque<int> d(4);
    for (int i = 0; i < 6; ++i) d.Append(i);   // 0..5
    SegmentedDeque<int> pat(4);
    pat.Append(2); pat.Append(3);
    EXPECT_EQ(d.FindSubsequence(pat), 2);
    SegmentedDeque<int> empty(4);
    EXPECT_EQ(d.FindSubsequence(empty), 0);
    SegmentedDeque<int> missing(4);
    missing.Append(9);
    EXPECT_EQ(d.FindSubsequence(missing), -1);
}

TEST(SegmentedDeque, Enumerator) {
    SegmentedDeque<int> d(3);
    for (int i = 1; i <= 5; ++i) d.Append(i);
    auto* en = d.GetEnumerator();
    int sum = 0;
    while (en->MoveNext()) sum += en->GetCurrent();
    EXPECT_EQ(sum, 15);
    en->Reset();
    EXPECT_TRUE(en->MoveNext());
    EXPECT_EQ(en->GetCurrent(), 1);
    delete en;
}

TEST(SegmentedDeque, OutputOperator) {
    SegmentedDeque<int> d(4);
    d.Append(1); d.Append(2);
    std::ostringstream oss;
    oss << d;
    EXPECT_EQ(oss.str(), "{ [_, _, 1, 2] }");
}

TEST(SegmentedDeque, SegmentLengthTooSmallThrows) {
    EXPECT_ANY_THROW(SegmentedDeque<int>(1));
}

TEST(SegmentedDeque, OperationsOnEmptyThrow) {
    SegmentedDeque<int> d(4);
    EXPECT_ANY_THROW(d.GetFirst());
    EXPECT_ANY_THROW(d.GetLast());
    EXPECT_ANY_THROW(d.PopFirst());
    EXPECT_ANY_THROW(d.PopLast());
    EXPECT_ANY_THROW(d.Get(0));
}

TEST(SegmentedDeque, OutOfRange) {
    SegmentedDeque<int> d(4);
    for (int i = 0; i < 3; ++i) d.Append(i);
    EXPECT_ANY_THROW(d.Get(5));
    EXPECT_ANY_THROW(d.Get(-1));
    EXPECT_ANY_THROW(d.InsertAt(1, 10));
    EXPECT_ANY_THROW(d.GetSubsequence(2, 1));
    EXPECT_ANY_THROW(d.GetSubsequence(0, 5));
}
