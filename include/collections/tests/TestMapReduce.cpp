#include <gtest/gtest.h>
#include "../ArraySequence.h"
#include "../ListSequence.h"
#include "../../utils/MapReduce.h"

// Map / Where / Reduce

TEST(MapReduce, MapArray) {
    int d[] = { 1,2,3 }; MutableArraySequence<int> s(d, 3);
    auto* r = Map<int, int>(
        &s,
        [](const int& x) { return x * 2; },
        []() -> Sequence<int>*{ return new MutableArraySequence<int>(); }
    );
    EXPECT_EQ(r->Get(0), 2);
    EXPECT_EQ(r->Get(2), 6);
    delete r;
}

TEST(MapReduce, WhereArray) {
    int d[] = { 1,2,3,4,5 }; MutableArraySequence<int> s(d, 5);
    auto* r = Where<int>(
        &s,
        [](const int& x) { return x % 2 == 0; },
        []() -> Sequence<int>*{ return new MutableArraySequence<int>(); }
    );
    EXPECT_EQ(r->GetLength(), 2);
    EXPECT_EQ(r->Get(0), 2);
    delete r;
}

TEST(MapReduce, ReduceSum) {
    int d[] = { 1,2,3,4 }; MutableArraySequence<int> s(d, 4);
    int sum = Reduce<int, int>(&s, [](const int& a, const int& x) { return a + x; }, 0);
    EXPECT_EQ(sum, 10);
}

TEST(MapReduce, MapOnEmpty) {
    MutableArraySequence<int> s;
    auto* r = Map<int, int>(
        &s,
        [](const int& x) { return x * 2; },
        []() -> Sequence<int>*{ return new MutableArraySequence<int>(); }
    );
    EXPECT_EQ(r->GetLength(), 0);
    delete r;
}

TEST(MapReduce, ReduceProduct) {
    int d[] = { 1,2,3 }; MutableArraySequence<int> s(d, 3);
    int p = Reduce<int, int>(&s, [](const int& a, const int& x) { return a * x; }, 1);
    EXPECT_EQ(p, 6);
}

TEST(MapReduce, MapList) {
    int d[] = { 1,2,3 }; MutableListSequence<int> s(d, 3);
    auto* r = Map<int, int>(
        &s,
        [](const int& x) { return x + 10; },
        []() -> Sequence<int>*{ return new MutableListSequence<int>(); }
    );
    EXPECT_EQ(r->GetLength(), 3);
    EXPECT_EQ(r->Get(0), 11);
    EXPECT_EQ(r->Get(2), 13);
    delete r;
}

TEST(MapReduce, WhereList) {
    int d[] = { 1,2,3,4,5 }; MutableListSequence<int> s(d, 5);
    auto* r = Where<int>(
        &s,
        [](const int& x) { return x > 3; },
        []() -> Sequence<int>*{ return new MutableListSequence<int>(); }
    );
    EXPECT_EQ(r->GetLength(), 2);
    EXPECT_EQ(r->Get(0), 4);
    delete r;
}

TEST(MapReduce, ReduceListSum) {
    int d[] = { 1,2,3,4 }; MutableListSequence<int> s(d, 4);
    int sum = Reduce<int, int>(&s, [](const int& a, const int& x) { return a + x; }, 0);
    EXPECT_EQ(sum, 10);
}

// Zip / Unzip

TEST(Zip, ArrayBasic) {
    int a[] = { 1,2,3 }, b[] = { 4,5,6 };
    MutableArraySequence<int> sa(a, 3), sb(b, 3);
    auto* z = Zip<int, int>(
        &sa,
        &sb,
        []() -> Sequence<Pair<int, int>>*{ return new MutableArraySequence<Pair<int, int>>(); }
    );
    EXPECT_EQ(z->GetLength(), 3);
    EXPECT_EQ(z->Get(0).first, 1);
    EXPECT_EQ(z->Get(0).second, 4);
    delete z;
}

TEST(Zip, TruncatesToShorter) {
    int a[] = { 1,2,3 }, b[] = { 4,5 };
    MutableArraySequence<int> sa(a, 3), sb(b, 2);
    auto* z = Zip<int, int>(
        &sa,
        &sb,
        []() -> Sequence<Pair<int, int>>*{ return new MutableArraySequence<Pair<int, int>>(); }
    );
    EXPECT_EQ(z->GetLength(), 2);
    delete z;
}

TEST(Zip, Unzip) {
    int a[] = { 1,2 }, b[] = { 3,4 };
    MutableArraySequence<int> sa(a, 2), sb(b, 2);
    auto* z = Zip<int, int>(
        &sa,
        &sb,
        []() -> Sequence<Pair<int, int>>*{ return new MutableArraySequence<Pair<int, int>>(); }
    );
    Sequence<int>* outA; Sequence<int>* outB;
    Unzip<int, int>(
        z,
        outA,
        outB,
        []() -> Sequence<int>*{ return new MutableArraySequence<int>(); },
        []() -> Sequence<int>*{ return new MutableArraySequence<int>(); }
    );
    EXPECT_EQ(outA->Get(0), 1);
    EXPECT_EQ(outB->Get(1), 4);
    delete z; delete outA; delete outB;
}

TEST(Zip, ListBasic) {
    int a[] = { 1,2,3 }, b[] = { 4,5,6 };
    MutableListSequence<int> sa(a, 3), sb(b, 3);
    auto* z = Zip<int, int>(
        &sa,
        &sb,
        []() -> Sequence<Pair<int, int>>*{ return new MutableListSequence<Pair<int, int>>(); }
    );
    EXPECT_EQ(z->GetLength(), 3);
    EXPECT_EQ(z->Get(0).first, 1);
    EXPECT_EQ(z->Get(2).second, 6);
    delete z;
}
