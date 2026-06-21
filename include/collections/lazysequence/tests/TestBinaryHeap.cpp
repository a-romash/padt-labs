#include <gtest/gtest.h>
#include <functional>
#include <stdexcept>
#include "../../BinaryHeap.h"

TEST(BinaryHeap, EmptyByDefault) {
    BinaryHeap<int> h;
    EXPECT_TRUE(h.IsEmpty());
    EXPECT_EQ(h.GetSize(), 0);
}

TEST(BinaryHeap, PushPopAscending) {
    BinaryHeap<int> h;
    int data[] = {5, 1, 4, 2, 8, 3};
    for (int x : data) h.Push(x);

    EXPECT_EQ(h.GetSize(), 6);
    int prev = -1000;
    while (!h.IsEmpty()) {
        int v = h.Pop();
        EXPECT_LE(prev, v);  // non-decreasing order
        prev = v;
    }
    EXPECT_TRUE(h.IsEmpty());
}

TEST(BinaryHeap, PeekReturnsMinWithoutRemoving) {
    BinaryHeap<int> h;
    h.Push(7);
    h.Push(3);
    h.Push(5);
    EXPECT_EQ(h.Peek(), 3);
    EXPECT_EQ(h.GetSize(), 3);
    EXPECT_EQ(h.Pop(), 3);
    EXPECT_EQ(h.Peek(), 5);
}

TEST(BinaryHeap, CustomComparatorMaxHeap) {
    BinaryHeap<int> h([](const int& a, const int& b) { return a > b; });
    int data[] = {5, 1, 4, 2, 8, 3};
    for (int x : data) h.Push(x);

    int prev = 1000;
    while (!h.IsEmpty()) {
        int v = h.Pop();
        EXPECT_GE(prev, v);  // non-increasing order
        prev = v;
    }
}

TEST(BinaryHeap, DuplicatesPreserved) {
    BinaryHeap<int> h;
    h.Push(2);
    h.Push(2);
    h.Push(1);
    h.Push(2);
    EXPECT_EQ(h.GetSize(), 4);
    EXPECT_EQ(h.Pop(), 1);
    EXPECT_EQ(h.Pop(), 2);
    EXPECT_EQ(h.Pop(), 2);
    EXPECT_EQ(h.Pop(), 2);
}

TEST(BinaryHeap, PopEmptyThrows) {
    BinaryHeap<int> h;
    EXPECT_THROW(h.Pop(), std::out_of_range);
}

TEST(BinaryHeap, PeekEmptyThrows) {
    BinaryHeap<int> h;
    EXPECT_THROW(h.Peek(), std::out_of_range);
}

TEST(BinaryHeap, EmptyComparatorThrows) {
    std::function<bool(const int&, const int&)> empty;
    EXPECT_THROW(BinaryHeap<int> h(empty), std::invalid_argument);
}

TEST(BinaryHeap, ManyElementsGrowCapacity) {
    BinaryHeap<int> h;
    for (int i = 100; i > 0; --i) h.Push(i);
    EXPECT_EQ(h.GetSize(), 100);
    for (int i = 1; i <= 100; ++i)
        EXPECT_EQ(h.Pop(), i);
}
