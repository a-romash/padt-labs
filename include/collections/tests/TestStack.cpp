#include <gtest/gtest.h>
#include <sstream>
#include "../Stack.h"
#include "../StackIO.h"

TEST(Stack, EmptyLengthZero) {
    Stack<int> s;
    EXPECT_EQ(s.GetLength(), 0);
    EXPECT_TRUE(s.IsEmpty());
}

TEST(Stack, PushIncreasesLength) {
    Stack<int> s;
    for (int i = 0; i < 5; ++i) s.Push(i);
    EXPECT_EQ(s.GetLength(), 5);
    EXPECT_FALSE(s.IsEmpty());
}

TEST(Stack, PeekReturnsTop) {
    Stack<int> s;
    s.Push(1); s.Push(2); s.Push(3);
    EXPECT_EQ(s.Peek(), 3);
    EXPECT_EQ(s.GetLength(), 3);
}

TEST(Stack, PopIsLifo) {
    Stack<int> s;
    for (int i = 0; i < 5; ++i) s.Push(i);
    EXPECT_EQ(s.Pop(), 4);
    EXPECT_EQ(s.Pop(), 3);
    EXPECT_EQ(s.Pop(), 2);
    EXPECT_EQ(s.GetLength(), 2);
    EXPECT_EQ(s.Peek(), 1);
}

TEST(Stack, PushPopAcrossSegments) {
    Stack<int> s(2);
    for (int i = 0; i < 10; ++i) s.Push(i);
    EXPECT_EQ(s.GetLength(), 10);
    for (int i = 9; i >= 0; --i)
        EXPECT_EQ(s.Pop(), i);
    EXPECT_TRUE(s.IsEmpty());
}

TEST(Stack, GetIsBottomToTop) {
    Stack<int> s;
    s.Push(10); s.Push(20); s.Push(30);
    EXPECT_EQ(s.Get(0), 10);
    EXPECT_EQ(s.Get(2), 30);
}

TEST(Stack, DrainThenReuse) {
    Stack<int> s;
    s.Push(1); s.Push(2);
    s.Pop(); s.Pop();
    EXPECT_TRUE(s.IsEmpty());
    s.Push(42);
    EXPECT_EQ(s.Peek(), 42);
    EXPECT_EQ(s.GetLength(), 1);
}

TEST(Stack, Clear) {
    Stack<int> s;
    for (int i = 0; i < 4; ++i) s.Push(i);
    s.Clear();
    EXPECT_TRUE(s.IsEmpty());
    EXPECT_EQ(s.GetLength(), 0);
    s.Push(7);
    EXPECT_EQ(s.Peek(), 7);
}

TEST(Stack, CopyIsIndependent) {
    Stack<int> a;
    a.Push(1); a.Push(2);
    Stack<int> b(a);
    b.Push(99);
    EXPECT_EQ(a.GetLength(), 2);
    EXPECT_EQ(b.GetLength(), 3);
    EXPECT_EQ(a.Peek(), 2);
    EXPECT_EQ(b.Peek(), 99);
}

TEST(Stack, AssignmentIsIndependent) {
    Stack<int> a;
    a.Push(1); a.Push(2); a.Push(3);
    Stack<int> b;
    b = a;
    a.Pop();
    EXPECT_EQ(b.GetLength(), 3);
    EXPECT_EQ(b.Peek(), 3);
}

TEST(Stack, Concat) {
    Stack<int> a;
    a.Push(1); a.Push(2);
    Stack<int> b;
    b.Push(3); b.Push(4);
    Stack<int> r = a.Concat(b);
    EXPECT_EQ(r.GetLength(), 4);
    EXPECT_EQ(r.Get(0), 1);
    EXPECT_EQ(r.Get(3), 4);
    EXPECT_EQ(r.Peek(), 4);
    EXPECT_EQ(a.GetLength(), 2);
}

TEST(Stack, Enumerator) {
    Stack<int> s;
    for (int i = 1; i <= 5; ++i) s.Push(i);
    auto* en = s.GetEnumerator();
    int sum = 0;
    while (en->MoveNext()) sum += en->GetCurrent();
    EXPECT_EQ(sum, 15);
    en->Reset();
    EXPECT_TRUE(en->MoveNext());
    EXPECT_EQ(en->GetCurrent(), 1);
    delete en;
}

TEST(Stack, OutputOperatorSegmented) {
    Stack<int> s(4);
    s.Push(1); s.Push(2);
    std::ostringstream oss;
    oss << s;
    EXPECT_EQ(oss.str(), "{ [_, _, 1, 2] }");
}

TEST(Stack, OutputOperatorEmpty) {
    Stack<int> s(4);
    std::ostringstream oss;
    oss << s;
    EXPECT_EQ(oss.str(), "{ [_, _, _, _] }");
}

TEST(Stack, SegmentLengthTooSmallThrows) {
    EXPECT_ANY_THROW(Stack<int>(1));
}

TEST(Stack, PopPeekOnEmptyThrow) {
    Stack<int> s;
    EXPECT_ANY_THROW(s.Pop());
    EXPECT_ANY_THROW(s.Peek());
}

TEST(Stack, GetOutOfRangeThrows) {
    Stack<int> s;
    s.Push(1); s.Push(2);
    EXPECT_ANY_THROW(s.Get(5));
    EXPECT_ANY_THROW(s.Get(-1));
}
