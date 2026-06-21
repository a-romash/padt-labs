#include <gtest/gtest.h>
#include "../ListSequence.h"
#include "../LinkedList.h"

// MutableListSequence

TEST(MutableListSequence, EmptyLengthZero) {
    MutableListSequence<int> s;
    EXPECT_EQ(s.GetLength(), 0);
}

TEST(MutableListSequence, FromArray) {
    int d[] = { 1,2,3 }; MutableListSequence<int> s(d, 3);
    EXPECT_EQ(s.GetLength(), 3);
    EXPECT_EQ(s.GetFirst(), 1);
    EXPECT_EQ(s.GetLast(), 3);
}

TEST(MutableListSequence, FromLinkedList) {
    int d[] = { 10,20,30 }; LinkedList<int> l(d, 3); MutableListSequence<int> s(l);
    EXPECT_EQ(s.GetLength(), 3);
    EXPECT_EQ(s.Get(1), 20);
}

TEST(MutableListSequence, CopyIndependent) {
    int d[] = { 1,2,3 }; MutableListSequence<int> a(d, 3); MutableListSequence<int> b(a);
    b.Append(99);
    EXPECT_EQ(a.GetLength(), 3);
}

TEST(MutableListSequence, Append) {
    MutableListSequence<int> s; s.Append(1); s.Append(2);
    EXPECT_EQ(s.GetLength(), 2);
    EXPECT_EQ(s.GetLast(), 2);
}

TEST(MutableListSequence, Prepend) {
    MutableListSequence<int> s; s.Append(2); s.Prepend(1);
    EXPECT_EQ(s.GetFirst(), 1);
    EXPECT_EQ(s.GetLength(), 2);
}

TEST(MutableListSequence, InsertAtMiddle) {
    int d[] = { 1,3 }; MutableListSequence<int> s(d, 2); s.InsertAt(2, 1);
    EXPECT_EQ(s.Get(1), 2);
    EXPECT_EQ(s.GetLength(), 3);
}

TEST(MutableListSequence, InsertAtFront) {
    int d[] = { 2,3 }; MutableListSequence<int> s(d, 2); s.InsertAt(1, 0);
    EXPECT_EQ(s.GetFirst(), 1);
}

TEST(MutableListSequence, InsertAtBack) {
    int d[] = { 1,2 }; MutableListSequence<int> s(d, 2); s.InsertAt(3, 2);
    EXPECT_EQ(s.GetLast(), 3);
}

TEST(MutableListSequence, GetSubsequence) {
    int d[] = { 10,20,30,40 }; MutableListSequence<int> s(d, 4);
    auto* sub = s.GetSubsequence(1, 2);
    EXPECT_EQ(sub->GetLength(), 2);
    EXPECT_EQ(sub->Get(0), 20);
    EXPECT_EQ(sub->Get(1), 30);
    delete sub;
}

TEST(MutableListSequence, GetSubsequenceFull) {
    int d[] = { 1,2,3 }; MutableListSequence<int> s(d, 3);
    auto* sub = s.GetSubsequence(0, 2);
    EXPECT_EQ(sub->GetLength(), 3);
    delete sub;
}

TEST(MutableListSequence, GetSubsequenceSingle) {
    int d[] = { 1,2,3 }; MutableListSequence<int> s(d, 3);
    auto* sub = s.GetSubsequence(2, 2);
    EXPECT_EQ(sub->GetLength(), 1);
    EXPECT_EQ(sub->Get(0), 3);
    delete sub;
}

TEST(MutableListSequence, Concat) {
    int a[] = { 1,2 }, b[] = { 3,4 }; MutableListSequence<int> sa(a, 2), sb(b, 2);
    auto* r = sa.Concat(sb);
    EXPECT_EQ(r->GetLength(), 4);
    EXPECT_EQ(r->Get(0), 1);
    EXPECT_EQ(r->Get(2), 3);
    delete r;
}

TEST(MutableListSequence, OperatorPlus) {
    int a[] = { 1,2 }, b[] = { 3,4 }; MutableListSequence<int> sa(a, 2), sb(b, 2);
    auto* r = sa + sb;
    EXPECT_EQ(r->GetLength(), 4);
    delete r;
}

TEST(MutableListSequence, Indexing) {
    int d[] = { 5,10,15 }; MutableListSequence<int> s(d, 3);
    EXPECT_EQ(s[0], 5);
    EXPECT_EQ(s[2], 15);
}

TEST(MutableListSequence, AppendReturnsThis) {
    int d[] = { 1,2,3 }; MutableListSequence<int> s(d, 3);
    Sequence<int>* r = s.Append(4);
    EXPECT_EQ(r, &s);
    EXPECT_EQ(s.GetLength(), 4);
}

TEST(MutableListSequence, GetFirstOnEmptyThrows) {
    MutableListSequence<int> s;
    EXPECT_ANY_THROW(s.GetFirst());
}

TEST(MutableListSequence, GetLastOnEmptyThrows) {
    MutableListSequence<int> s;
    EXPECT_ANY_THROW(s.GetLast());
}

TEST(MutableListSequence, GetOutOfRange) {
    int d[] = { 1,2 }; MutableListSequence<int> s(d, 2);
    EXPECT_ANY_THROW(s.Get(-1));
    EXPECT_ANY_THROW(s.Get(5));
}

TEST(MutableListSequence, InsertAtOnEmptyThrows) {
    MutableListSequence<int> s;
    EXPECT_ANY_THROW(s.InsertAt(1, 5));
}

TEST(MutableListSequence, GetSubsequenceInvalid) {
    int d[] = { 1,2,3 }; MutableListSequence<int> s(d, 3);
    EXPECT_ANY_THROW(s.GetSubsequence(2, 1));
    EXPECT_ANY_THROW(s.GetSubsequence(-1, 1));
    EXPECT_ANY_THROW(s.GetSubsequence(0, 5));
}

TEST(MutableListSequence, NullThrows) {
    EXPECT_ANY_THROW(MutableListSequence<int>(nullptr, 3));
}

TEST(MutableListSequence, NullNegThrows) {
    EXPECT_ANY_THROW(MutableListSequence<int>(nullptr, -1));
}

// ImmutableListSequence

TEST(ImmutableListSequence, AppendReturnsNew) {
    int d[] = { 1,2,3 }; ImmutableListSequence<int> s(d, 3);
    Sequence<int>* r = s.Append(4);
    EXPECT_NE(r, &s);
    EXPECT_EQ(s.GetLength(), 3);
    EXPECT_EQ(r->GetLength(), 4);
    delete r;
}

TEST(ImmutableListSequence, PrependReturnsNew) {
    int d[] = { 2,3 }; ImmutableListSequence<int> s(d, 2);
    Sequence<int>* r = s.Prepend(1);
    EXPECT_NE(r, &s);
    EXPECT_EQ(s.GetFirst(), 2);
    EXPECT_EQ(r->GetFirst(), 1);
    delete r;
}

TEST(ImmutableListSequence, InsertAtReturnsNew) {
    int d[] = { 1,3 }; ImmutableListSequence<int> s(d, 2);
    Sequence<int>* r = s.InsertAt(2, 1);
    EXPECT_NE(r, &s);
    EXPECT_EQ(s.GetLength(), 2);
    EXPECT_EQ(r->GetLength(), 3);
    delete r;
}

TEST(ImmutableListSequence, MultipleOpsOriginalUnchanged) {
    int d[] = { 1,2,3 }; ImmutableListSequence<int> s(d, 3);
    auto* r1 = s.Append(4); auto* r2 = s.Prepend(0);
    EXPECT_EQ(s.GetLength(), 3);
    EXPECT_EQ(r1->GetLength(), 4);
    EXPECT_EQ(r2->GetLength(), 4);
    delete r1; delete r2;
}

// Builder (ListSequence)

TEST(ListSequenceBuilder, Chain) {
    auto* s = MutableListSequence<int>::Builder()
        .Append(1).Append(2).Append(3).Build();
    EXPECT_EQ(s->GetLength(), 3);
    EXPECT_EQ(s->Get(0), 1);
    EXPECT_EQ(s->Get(2), 3);
    delete s;
}

TEST(ListSequenceBuilder, AppendAll) {
    int d[] = { 10,20,30 };
    auto* s = MutableListSequence<int>::Builder().AppendAll(d, 3).Build();
    EXPECT_EQ(s->GetLength(), 3);
    EXPECT_EQ(s->Get(1), 20);
    delete s;
}

TEST(ListSequenceBuilder, AppendSequence) {
    int d[] = { 1,2,3 }; MutableListSequence<int> src(d, 3);
    auto* s = MutableListSequence<int>::Builder().AppendSequence(src).Build();
    EXPECT_EQ(s->GetLength(), 3);
    EXPECT_EQ(s->Get(2), 3);
    delete s;
}

TEST(ListSequenceBuilder, AppendAllNullThrows) {
    MutableListSequence<int>::Builder b;
    EXPECT_ANY_THROW(b.AppendAll(nullptr, 3));
}

TEST(ListSequenceBuilder, BuildResetsBuilder) {
    auto builder = MutableListSequence<int>::Builder();
    builder.Append(1).Append(2);
    auto* first = builder.Build();
    builder.Append(99);
    auto* second = builder.Build();
    EXPECT_EQ(first->GetLength(), 2);
    EXPECT_EQ(second->GetLength(), 1);
    delete first; delete second;
}

// Enumerator (ListSequence / LinkedList)

TEST(ListSequenceEnumerator, IterateSum) {
    int d[] = { 1,2,3 }; MutableListSequence<int> s(d, 3);
    auto* en = s.GetEnumerator(); int sum = 0;
    while (en->MoveNext()) sum += en->GetCurrent();
    EXPECT_EQ(sum, 6);
    delete en;
}

TEST(ListSequenceEnumerator, ResetRestarts) {
    int d[] = { 5,10 }; MutableListSequence<int> s(d, 2);
    auto* en = s.GetEnumerator();
    en->MoveNext(); en->Reset(); en->MoveNext();
    EXPECT_EQ(en->GetCurrent(), 5);
    delete en;
}

TEST(ListSequenceEnumerator, GetCurrentBeforeMoveNextThrows) {
    int d[] = { 1 }; MutableListSequence<int> s(d, 1);
    auto* en = s.GetEnumerator();
    EXPECT_ANY_THROW(en->GetCurrent());
    delete en;
}

TEST(ListSequenceEnumerator, MoveNextOnEmptyFalse) {
    MutableListSequence<int> s;
    auto* en = s.GetEnumerator();
    EXPECT_FALSE(en->MoveNext());
    delete en;
}
