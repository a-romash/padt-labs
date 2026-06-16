#include <gtest/gtest.h>
#include "../DynamicArray.h"

TEST(DynamicArray, EmptyHasSizeZero) {
    DynamicArray<int> arr(0);
    EXPECT_EQ(arr.GetSize(), 0);
}

TEST(DynamicArray, FromArray) {
    int d[] = { 1,2,3 }; DynamicArray<int> arr(d, 3);
    EXPECT_EQ(arr.GetSize(), 3);
    EXPECT_EQ(arr.Get(0), 1);
    EXPECT_EQ(arr.Get(2), 3);
}

TEST(DynamicArray, CopyIsIndependent) {
    int d[] = { 10,20 }; DynamicArray<int> a(d, 2); DynamicArray<int> b(a);
    b.Set(0, 99);
    EXPECT_EQ(a.Get(0), 10);
}

TEST(DynamicArray, SetAndGet) {
    DynamicArray<int> arr(3); arr.Set(1, 42);
    EXPECT_EQ(arr.Get(1), 42);
}

TEST(DynamicArray, ResizeUp) {
    int d[] = { 1,2,3 }; DynamicArray<int> arr(d, 3); arr.Resize(5);
    EXPECT_EQ(arr.GetSize(), 5);
    EXPECT_EQ(arr.Get(0), 1);
}

TEST(DynamicArray, ResizeDown) {
    int d[] = { 1,2,3,4,5 }; DynamicArray<int> arr(d, 5); arr.Resize(2);
    EXPECT_EQ(arr.GetSize(), 2);
    EXPECT_EQ(arr.Get(1), 2);
}

TEST(DynamicArray, ResizeToZero) {
    int d[] = { 1,2 }; DynamicArray<int> arr(d, 2); arr.Resize(0);
    EXPECT_EQ(arr.GetSize(), 0);
}

TEST(DynamicArray, SingleElement) {
    int d[] = { 7 }; DynamicArray<int> arr(d, 1);
    EXPECT_EQ(arr.Get(0), 7);
}

TEST(DynamicArray, NegativeSizeThrows) {
    EXPECT_ANY_THROW(DynamicArray<int>(-1));
}

TEST(DynamicArray, NullPtrThrows) {
    EXPECT_ANY_THROW(DynamicArray<int>(nullptr, 3));
}

TEST(DynamicArray, GetOutOfRange) {
    DynamicArray<int> arr(3);
    EXPECT_ANY_THROW(arr.Get(3));
    EXPECT_ANY_THROW(arr.Get(-1));
}

TEST(DynamicArray, SetOutOfRange) {
    DynamicArray<int> arr(2);
    EXPECT_ANY_THROW(arr.Set(5, 1));
}

TEST(DynamicArray, ResizeNegative) {
    DynamicArray<int> arr(2);
    EXPECT_ANY_THROW(arr.Resize(-1));
}

TEST(DynamicArray, EnumeratorSum) {
    int d[] = { 1,2,3 }; DynamicArray<int> arr(d, 3);
    auto* en = arr.GetEnumerator();
    int sum = 0;
    while (en->MoveNext()) sum += en->GetCurrent();
    EXPECT_EQ(sum, 6);
    delete en;
}

TEST(DynamicArray, EnumeratorReset) {
    int d[] = { 5,10 }; DynamicArray<int> arr(d, 2);
    auto* en = arr.GetEnumerator();
    EXPECT_TRUE(en->MoveNext());
    EXPECT_EQ(en->GetCurrent(), 5);
    en->Reset();
    EXPECT_TRUE(en->MoveNext());
    EXPECT_EQ(en->GetCurrent(), 5);
    delete en;
}

TEST(DynamicArray, EnumeratorGetCurrentBeforeMoveNextThrows) {
    int d[] = { 1 }; DynamicArray<int> arr(d, 1);
    auto* en = arr.GetEnumerator();
    EXPECT_ANY_THROW(en->GetCurrent());
    delete en;
}
