#include <gtest/gtest.h>
#include <string>
#include "../StreamSort.h"
#include "../../ArraySequence.h"

// Задание 10.2: сортировка потока данных через бинарную кучу.

TEST(StreamSort, Ascending) {
    int d[] = {5, 2, 9, 1, 5, 6, 3};
    MutableArraySequence<int> src(d, 7);
    MutableArraySequence<int> dst;

    SequenceReadStream<int> in(src);
    SequenceWriteStream<int> out(dst);
    SortStream<int>(in, out);

    int expected[] = {1, 2, 3, 5, 5, 6, 9};
    ASSERT_EQ(dst.GetLength(), 7);
    for (int i = 0; i < 7; ++i)
        EXPECT_EQ(dst.Get(i), expected[i]);
}

TEST(StreamSort, Descending) {
    int d[] = {5, 2, 9, 1, 6, 3};
    MutableArraySequence<int> src(d, 6);
    MutableArraySequence<int> dst;

    SequenceReadStream<int> in(src);
    SequenceWriteStream<int> out(dst);
    SortStream<int>(in, out, [](const int& a, const int& b) { return a > b; });

    int expected[] = {9, 6, 5, 3, 2, 1};
    ASSERT_EQ(dst.GetLength(), 6);
    for (int i = 0; i < 6; ++i)
        EXPECT_EQ(dst.Get(i), expected[i]);
}

TEST(StreamSort, EmptyInput) {
    MutableArraySequence<int> src;
    MutableArraySequence<int> dst;

    SequenceReadStream<int> in(src);
    SequenceWriteStream<int> out(dst);
    SortStream<int>(in, out);

    EXPECT_EQ(dst.GetLength(), 0);
}

TEST(StreamSort, SingleElement) {
    int d[] = {42};
    MutableArraySequence<int> src(d, 1);
    MutableArraySequence<int> dst;

    SequenceReadStream<int> in(src);
    SequenceWriteStream<int> out(dst);
    SortStream<int>(in, out);

    ASSERT_EQ(dst.GetLength(), 1);
    EXPECT_EQ(dst.Get(0), 42);
}

TEST(StreamSort, FromStringStream) {
    StringReadStream<int> in("3 1 2 1 9 4", [](const std::string& s) { return std::stoi(s); });
    MutableArraySequence<int> dst;
    SequenceWriteStream<int> out(dst);
    SortStream<int>(in, out);

    int expected[] = {1, 1, 2, 3, 4, 9};
    ASSERT_EQ(dst.GetLength(), 6);
    for (int i = 0; i < 6; ++i)
        EXPECT_EQ(dst.Get(i), expected[i]);
}
