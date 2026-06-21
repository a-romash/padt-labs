#include <gtest/gtest.h>
#include "../DoublyLinkedList.h"

TEST(DoublyLinkedList, EmptyLengthZero) {
    DoublyLinkedList<int> l;
    EXPECT_EQ(l.GetLength(), 0);
}

TEST(DoublyLinkedList, FromArray) {
    int d[] = { 1,2,3 }; DoublyLinkedList<int> l(d, 3);
    EXPECT_EQ(l.GetLength(), 3);
    EXPECT_EQ(l.GetFirst(), 1);
    EXPECT_EQ(l.GetLast(), 3);
}

TEST(DoublyLinkedList, Append) {
    DoublyLinkedList<int> l; l.Append(10); l.Append(20);
    EXPECT_EQ(l.Get(0), 10);
    EXPECT_EQ(l.Get(1), 20);
}

TEST(DoublyLinkedList, Prepend) {
    DoublyLinkedList<int> l; l.Append(2); l.Prepend(1);
    EXPECT_EQ(l.GetFirst(), 1);
    EXPECT_EQ(l.GetLength(), 2);
}

TEST(DoublyLinkedList, InsertAtMiddle) {
    int d[] = { 1,3 }; DoublyLinkedList<int> l(d, 2); l.InsertAt(2, 1);
    EXPECT_EQ(l.Get(1), 2);
    EXPECT_EQ(l.GetLength(), 3);
}

TEST(DoublyLinkedList, InsertAtFront) {
    int d[] = { 2,3 }; DoublyLinkedList<int> l(d, 2); l.InsertAt(1, 0);
    EXPECT_EQ(l.GetFirst(), 1);
}

TEST(DoublyLinkedList, InsertAtBack) {
    int d[] = { 1,2 }; DoublyLinkedList<int> l(d, 2); l.InsertAt(3, 2);
    EXPECT_EQ(l.GetLast(), 3);
}

TEST(DoublyLinkedList, DelFront) {
    int d[] = { 1,2,3 }; DoublyLinkedList<int> l(d, 3); l.Del(0);
    EXPECT_EQ(l.GetLength(), 2);
    EXPECT_EQ(l.GetFirst(), 2);
}

TEST(DoublyLinkedList, DelBack) {
    int d[] = { 1,2,3 }; DoublyLinkedList<int> l(d, 3); l.Del(2);
    EXPECT_EQ(l.GetLength(), 2);
    EXPECT_EQ(l.GetLast(), 2);
}

TEST(DoublyLinkedList, DelMiddle) {
    int d[] = { 1,2,3 }; DoublyLinkedList<int> l(d, 3); l.Del(1);
    EXPECT_EQ(l.GetLength(), 2);
    EXPECT_EQ(l.Get(1), 3);
}

TEST(DoublyLinkedList, DelAllThenReuse) {
    int d[] = { 1,2,3 }; DoublyLinkedList<int> l(d, 3); l.Del(0); l.Del(0); l.Del(0);
    EXPECT_EQ(l.GetLength(), 0);
    l.Append(7);
    EXPECT_EQ(l.GetFirst(), 7);
}

TEST(DoublyLinkedList, MutableAccessors) {
    int d[] = { 5,6,7 }; DoublyLinkedList<int> l(d, 3);
    l.GetFirst() = 50; l.GetLast() = 70; l.Get(1) = 60;
    EXPECT_EQ(l.Get(0), 50);
    EXPECT_EQ(l.Get(1), 60);
    EXPECT_EQ(l.Get(2), 70);
}

TEST(DoublyLinkedList, GetSubList) {
    int d[] = { 10,20,30,40 }; DoublyLinkedList<int> l(d, 4);
    auto sub = l.GetSubList(1, 2);
    EXPECT_EQ(sub.GetLength(), 2);
    EXPECT_EQ(sub.Get(0), 20);
}

TEST(DoublyLinkedList, Concat) {
    int a[] = { 1,2 }, b[] = { 3,4 }; DoublyLinkedList<int> la(a, 2), lb(b, 2);
    auto r = la.Concat(lb);
    EXPECT_EQ(r.GetLength(), 4);
    EXPECT_EQ(r.Get(2), 3);
}

TEST(DoublyLinkedList, CopyConstructorIndependent) {
    int d[] = { 1,2,3 }; DoublyLinkedList<int> a(d, 3); DoublyLinkedList<int> b(a);
    b.Append(99);
    EXPECT_EQ(a.GetLength(), 3);
}

TEST(DoublyLinkedList, AssignmentIndependent) {
    int d[] = { 1,2,3 }; DoublyLinkedList<int> a(d, 3); DoublyLinkedList<int> b; b = a;
    b.Append(99);
    EXPECT_EQ(a.GetLength(), 3);
}

TEST(DoublyLinkedList, EnumeratorSum) {
    int d[] = { 1,2,3 }; DoublyLinkedList<int> l(d, 3);
    auto* en = l.GetEnumerator();
    int sum = 0;
    while (en->MoveNext()) sum += en->GetCurrent();
    EXPECT_EQ(sum, 6);
    delete en;
}

TEST(DoublyLinkedList, EnumeratorReset) {
    int d[] = { 5,10 }; DoublyLinkedList<int> l(d, 2);
    auto* en = l.GetEnumerator();
    EXPECT_TRUE(en->MoveNext());
    EXPECT_EQ(en->GetCurrent(), 5);
    en->Reset();
    EXPECT_TRUE(en->MoveNext());
    EXPECT_EQ(en->GetCurrent(), 5);
    delete en;
}

TEST(DoublyLinkedList, GetFirstLastOnEmptyThrows) {
    DoublyLinkedList<int> l;
    EXPECT_ANY_THROW(l.GetFirst());
    EXPECT_ANY_THROW(l.GetLast());
}

TEST(DoublyLinkedList, OutOfRange) {
    int d[] = { 1,2 }; DoublyLinkedList<int> l(d, 2);
    EXPECT_ANY_THROW(l.Get(5));
    EXPECT_ANY_THROW(l.Get(-1));
    EXPECT_ANY_THROW(l.Del(5));
}

TEST(DoublyLinkedList, InsertAtOnEmptyThrows) {
    DoublyLinkedList<int> l;
    EXPECT_ANY_THROW(l.InsertAt(1, 5));
}

TEST(DoublyLinkedList, NullNegThrows) {
    EXPECT_ANY_THROW(DoublyLinkedList<int>(nullptr, -1));
}
