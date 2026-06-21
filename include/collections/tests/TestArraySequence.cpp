#include <gtest/gtest.h>
#include "../ArraySequence.h"

// MutableArraySequence

TEST(MutableArraySequence, EmptyLengthZero) {
    MutableArraySequence<int> s;
    EXPECT_EQ(s.GetLength(), 0);
}

TEST(MutableArraySequence, FromArray) {
    int d[] = { 1,2,3 }; MutableArraySequence<int> s(d, 3);
    EXPECT_EQ(s.GetLength(), 3);
    EXPECT_EQ(s.GetFirst(), 1);
    EXPECT_EQ(s.GetLast(), 3);
}

TEST(MutableArraySequence, Append) {
    MutableArraySequence<int> s; s.Append(1); s.Append(2);
    EXPECT_EQ(s.GetLength(), 2);
    EXPECT_EQ(s.Get(1), 2);
}

TEST(MutableArraySequence, Prepend) {
    MutableArraySequence<int> s; s.Append(2); s.Prepend(1);
    EXPECT_EQ(s.GetFirst(), 1);
    EXPECT_EQ(s.GetLength(), 2);
}

TEST(MutableArraySequence, InsertAt) {
    int d[] = { 1,3 }; MutableArraySequence<int> s(d, 2); s.InsertAt(2, 1);
    EXPECT_EQ(s.Get(1), 2);
    EXPECT_EQ(s.GetLength(), 3);
}

TEST(MutableArraySequence, GetSubsequence) {
    int d[] = { 10,20,30,40 }; MutableArraySequence<int> s(d, 4);
    auto* sub = s.GetSubsequence(1, 2);
    EXPECT_EQ(sub->GetLength(), 2);
    EXPECT_EQ(sub->Get(0), 20);
    delete sub;
}

TEST(MutableArraySequence, Concat) {
    int a[] = { 1,2 }, b[] = { 3,4 }; MutableArraySequence<int> sa(a, 2), sb(b, 2);
    auto* r = sa.Concat(sb);
    EXPECT_EQ(r->GetLength(), 4);
    EXPECT_EQ(r->Get(2), 3);
    delete r;
}

TEST(MutableArraySequence, Indexing) {
    int d[] = { 5,10,15 }; MutableArraySequence<int> s(d, 3);
    EXPECT_EQ(s[0], 5);
    EXPECT_EQ(s[2], 15);
}

TEST(MutableArraySequence, AppendReturnsThis) {
    int d[] = { 1,2,3 }; MutableArraySequence<int> s(d, 3);
    Sequence<int>* r = s.Append(4);
    EXPECT_EQ(r, &s);
    EXPECT_EQ(s.GetLength(), 4);
}

TEST(MutableArraySequence, HundredElements) {
    MutableArraySequence<int> s;
    for (int i = 0; i < 100; ++i) s.Append(i);
    EXPECT_EQ(s.GetLength(), 100);
    EXPECT_EQ(s.Get(99), 99);
}

TEST(MutableArraySequence, GetSubsequenceFull) {
    int d[] = { 1,2,3 }; MutableArraySequence<int> s(d, 3);
    auto* sub = s.GetSubsequence(0, 2);
    EXPECT_EQ(sub->GetLength(), 3);
    delete sub;
}

TEST(MutableArraySequence, GetSubsequenceSingle) {
    int d[] = { 1,2,3 }; MutableArraySequence<int> s(d, 3);
    auto* sub = s.GetSubsequence(1, 1);
    EXPECT_EQ(sub->GetLength(), 1);
    EXPECT_EQ(sub->Get(0), 2);
    delete sub;
}

TEST(MutableArraySequence, GetFirstOnEmptyThrows) {
    MutableArraySequence<int> s;
    EXPECT_ANY_THROW(s.GetFirst());
}

TEST(MutableArraySequence, GetLastOnEmptyThrows) {
    MutableArraySequence<int> s;
    EXPECT_ANY_THROW(s.GetLast());
}

TEST(MutableArraySequence, GetOutOfRange) {
    int d[] = { 1,2 }; MutableArraySequence<int> s(d, 2);
    EXPECT_ANY_THROW(s.Get(5));
    EXPECT_ANY_THROW(s.Get(-1));
}

TEST(MutableArraySequence, InsertAtOnEmptyThrows) {
    MutableArraySequence<int> s;
    EXPECT_ANY_THROW(s.InsertAt(1, 5));
}

TEST(MutableArraySequence, GetSubsequenceInvalid) {
    int d[] = { 1,2,3 }; MutableArraySequence<int> s(d, 3);
    EXPECT_ANY_THROW(s.GetSubsequence(2, 1));
    EXPECT_ANY_THROW(s.GetSubsequence(-1, 1));
    EXPECT_ANY_THROW(s.GetSubsequence(0, 5));
}

TEST(MutableArraySequence, NullPtrThrows) {
    EXPECT_ANY_THROW(MutableArraySequence<int>(nullptr, 3));
}

// ImmutableArraySequence

TEST(ImmutableArraySequence, AppendReturnsNew) {
    int d[] = { 1,2,3 }; ImmutableArraySequence<int> s(d, 3);
    Sequence<int>* r = s.Append(4);
    EXPECT_NE(r, &s);
    EXPECT_EQ(s.GetLength(), 3);
    EXPECT_EQ(r->GetLength(), 4);
    delete r;
}

TEST(ImmutableArraySequence, PrependReturnsNew) {
    int d[] = { 2,3 }; ImmutableArraySequence<int> s(d, 2);
    Sequence<int>* r = s.Prepend(1);
    EXPECT_NE(r, &s);
    EXPECT_EQ(s.GetFirst(), 2);
    EXPECT_EQ(r->GetFirst(), 1);
    delete r;
}

TEST(ImmutableArraySequence, InsertAtReturnsNew) {
    int d[] = { 1,3 }; ImmutableArraySequence<int> s(d, 2);
    Sequence<int>* r = s.InsertAt(2, 1);
    EXPECT_EQ(s.GetLength(), 2);
    EXPECT_EQ(r->GetLength(), 3);
    delete r;
}

// Builder (ArraySequence)

TEST(ArraySequenceBuilder, Chain) {
    auto* s = MutableArraySequence<int>::Builder()
        .Append(1).Append(2).Append(3).Build();
    EXPECT_EQ(s->GetLength(), 3);
    EXPECT_EQ(s->Get(0), 1);
    delete s;
}

TEST(ArraySequenceBuilder, AppendAll) {
    int d[] = { 10,20,30 };
    auto* s = MutableArraySequence<int>::Builder().AppendAll(d, 3).Build();
    EXPECT_EQ(s->GetLength(), 3);
    EXPECT_EQ(s->Get(2), 30);
    delete s;
}

TEST(ArraySequenceBuilder, AppendSequence) {
    int d[] = { 1,2 }; MutableArraySequence<int> src(d, 2);
    auto* s = MutableArraySequence<int>::Builder().AppendSequence(src).Build();
    EXPECT_EQ(s->GetLength(), 2);
    delete s;
}

TEST(ArraySequenceBuilder, AppendAllNullThrows) {
    MutableArraySequence<int>::Builder b;
    EXPECT_ANY_THROW(b.AppendAll(nullptr, 3));
}

// Enumerator (ArraySequence / DynamicArray)

TEST(ArraySequenceEnumerator, IterateSum) {
    int d[] = { 1,2,3 }; MutableArraySequence<int> s(d, 3);
    auto* en = s.GetEnumerator(); int sum = 0;
    while (en->MoveNext()) sum += en->GetCurrent();
    EXPECT_EQ(sum, 6);
    delete en;
}

TEST(ArraySequenceEnumerator, ResetRestarts) {
    int d[] = { 5,10 }; MutableArraySequence<int> s(d, 2);
    auto* en = s.GetEnumerator();
    en->MoveNext(); en->Reset(); en->MoveNext();
    EXPECT_EQ(en->GetCurrent(), 5);
    delete en;
}

TEST(ArraySequenceEnumerator, GetCurrentBeforeMoveNextThrows) {
    int d[] = { 1 }; MutableArraySequence<int> s(d, 1);
    auto* en = s.GetEnumerator();
    EXPECT_ANY_THROW(en->GetCurrent());
    delete en;
}

TEST(ArraySequenceEnumerator, MoveNextOnEmptyFalse) {
    MutableArraySequence<int> s;
    auto* en = s.GetEnumerator();
    EXPECT_FALSE(en->MoveNext());
    delete en;
}
