#include <gtest/gtest.h>
#include "../LinkedList.h"

TEST(LinkedList, EmptyLengthZero) {
    LinkedList<int> l;
    EXPECT_EQ(l.GetLength(), 0);
}

TEST(LinkedList, FromArray) {
    int d[] = { 1,2,3 }; LinkedList<int> l(d, 3);
    EXPECT_EQ(l.GetLength(), 3);
    EXPECT_EQ(l.GetFirst(), 1);
    EXPECT_EQ(l.GetLast(), 3);
}

TEST(LinkedList, Append) {
    LinkedList<int> l; l.Append(10); l.Append(20);
    EXPECT_EQ(l.Get(0), 10);
    EXPECT_EQ(l.Get(1), 20);
}

TEST(LinkedList, Prepend) {
    LinkedList<int> l; l.Append(2); l.Prepend(1);
    EXPECT_EQ(l.GetFirst(), 1);
}

TEST(LinkedList, InsertAtMiddle) {
    int d[] = { 1,3 }; LinkedList<int> l(d, 2); l.InsertAt(2, 1);
    EXPECT_EQ(l.Get(1), 2);
    EXPECT_EQ(l.GetLength(), 3);
}

TEST(LinkedList, GetSubList) {
    int d[] = { 10,20,30,40 }; LinkedList<int> l(d, 4);
    auto sub = l.GetSubList(1, 2);
    EXPECT_EQ(sub.GetLength(), 2);
    EXPECT_EQ(sub.Get(0), 20);
}

TEST(LinkedList, Concat) {
    int a[] = { 1,2 }, b[] = { 3,4 }; LinkedList<int> la(a, 2), lb(b, 2);
    auto r = la.Concat(lb);
    EXPECT_EQ(r.GetLength(), 4);
    EXPECT_EQ(r.Get(2), 3);
}

TEST(LinkedList, CopyConstructorIndependent) {
    int d[] = { 1,2,3 }; LinkedList<int> a(d, 3); LinkedList<int> b(a);
    b.Append(99);
    EXPECT_EQ(a.GetLength(), 3);
}

TEST(LinkedList, SingleAppend) {
    LinkedList<int> l; l.Append(42);
    EXPECT_EQ(l.GetFirst(), 42);
    EXPECT_EQ(l.GetLast(), 42);
}

TEST(LinkedList, InsertAtFront) {
    int d[] = { 2,3 }; LinkedList<int> l(d, 2); l.InsertAt(1, 0);
    EXPECT_EQ(l.GetFirst(), 1);
}

TEST(LinkedList, InsertAtBack) {
    int d[] = { 1,2 }; LinkedList<int> l(d, 2); l.InsertAt(3, 2);
    EXPECT_EQ(l.GetLast(), 3);
}

TEST(LinkedList, GetFirstOnEmptyThrows) {
    LinkedList<int> l;
    EXPECT_ANY_THROW(l.GetFirst());
}

TEST(LinkedList, GetLastOnEmptyThrows) {
    LinkedList<int> l;
    EXPECT_ANY_THROW(l.GetLast());
}

TEST(LinkedList, GetOutOfRange) {
    int d[] = { 1,2 }; LinkedList<int> l(d, 2);
    EXPECT_ANY_THROW(l.Get(5));
    EXPECT_ANY_THROW(l.Get(-1));
}

TEST(LinkedList, InsertAtOnEmptyThrows) {
    LinkedList<int> l;
    EXPECT_ANY_THROW(l.InsertAt(1, 5));
}

TEST(LinkedList, NullNegThrows) {
    EXPECT_ANY_THROW(LinkedList<int>(nullptr, -1));
}

TEST(LinkedList, EnumeratorSum) {
    int d[] = { 1,2,3 }; LinkedList<int> l(d, 3);
    auto* en = l.GetEnumerator();
    int sum = 0;
    while (en->MoveNext()) sum += en->GetCurrent();
    EXPECT_EQ(sum, 6);
    delete en;
}

TEST(LinkedList, EnumeratorReset) {
    int d[] = { 5,10 }; LinkedList<int> l(d, 2);
    auto* en = l.GetEnumerator();
    EXPECT_TRUE(en->MoveNext());
    EXPECT_EQ(en->GetCurrent(), 5);
    en->Reset();
    EXPECT_TRUE(en->MoveNext());
    EXPECT_EQ(en->GetCurrent(), 5);
    delete en;
}

TEST(LinkedList, EnumeratorBoundaries) {
    int d[] = { 1 }; LinkedList<int> l(d, 1);
    auto* en = l.GetEnumerator();
    EXPECT_ANY_THROW(en->GetCurrent());
    EXPECT_TRUE(en->MoveNext());
    EXPECT_FALSE(en->MoveNext());
    EXPECT_ANY_THROW(en->GetCurrent());
    delete en;
}
