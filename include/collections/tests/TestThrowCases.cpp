#include <gtest/gtest.h>
#include <functional>
#include "../DynamicArray.h"
#include "../LinkedList.h"
#include "../ArraySequence.h"
#include "../ListSequence.h"
#include "../../utils/MapReduce.h"

TEST(ThrowCases, DynamicArrayEmpty) {
    DynamicArray<int> arr;
    EXPECT_ANY_THROW(arr.Get(0));
    EXPECT_ANY_THROW(arr.Get(-1));
    EXPECT_ANY_THROW(arr.Set(0, 10));
    EXPECT_ANY_THROW(arr.Resize(-1));
}

TEST(ThrowCases, DynamicArrayBounds) {
    int data[] = { 1, 2, 3 };
    DynamicArray<int> arr(data, 3);

    EXPECT_ANY_THROW(arr.Get(3));
    EXPECT_ANY_THROW(arr.Set(3, 10));

    auto* en = arr.GetEnumerator();
    EXPECT_ANY_THROW(en->GetCurrent());
    while (en->MoveNext()) {
    }
    EXPECT_ANY_THROW(en->GetCurrent());
    delete en;
}

TEST(ThrowCases, LinkedListEmpty) {
    LinkedList<int> list;
    EXPECT_ANY_THROW(list.GetFirst());
    EXPECT_ANY_THROW(list.GetLast());
    EXPECT_ANY_THROW(list.Get(0));
    EXPECT_ANY_THROW(list.Get(-1));
    EXPECT_ANY_THROW(list.InsertAt(10, 1));

    auto* en = list.GetEnumerator();
    EXPECT_ANY_THROW(en->GetCurrent());
    en->MoveNext();
    EXPECT_ANY_THROW(en->GetCurrent());
    delete en;
}

TEST(ThrowCases, LinkedListBounds) {
    int data[] = { 1, 2, 3 };
    LinkedList<int> list(data, 3);

    EXPECT_ANY_THROW(list.Get(3));
    EXPECT_ANY_THROW(list.InsertAt(10, 4));
    EXPECT_ANY_THROW(list.GetSubList(-1, 1));
    EXPECT_ANY_THROW(list.GetSubList(0, 3));
    EXPECT_ANY_THROW(list.GetSubList(2, 1));

    auto* en = list.GetEnumerator();
    while (en->MoveNext()) {
    }
    EXPECT_ANY_THROW(en->GetCurrent());
    delete en;
}

TEST(ThrowCases, ArraySequenceEmpty) {
    MutableArraySequence<int> seq;
    EXPECT_ANY_THROW(seq.GetFirst());
    EXPECT_ANY_THROW(seq.GetLast());
    EXPECT_ANY_THROW(seq.Get(0));
    EXPECT_ANY_THROW(seq.Get(-1));
    EXPECT_ANY_THROW(seq.InsertAt(10, 1));
    EXPECT_ANY_THROW(seq.GetSubsequence(0, 0));

    auto* en = seq.GetEnumerator();
    EXPECT_ANY_THROW(en->GetCurrent());
    en->MoveNext();
    EXPECT_ANY_THROW(en->GetCurrent());
    delete en;
}

TEST(ThrowCases, ArraySequenceBounds) {
    int data[] = { 1, 2, 3 };
    MutableArraySequence<int> seq(data, 3);

    EXPECT_ANY_THROW(seq.Get(3));
    EXPECT_ANY_THROW(seq.GetSubsequence(-1, 1));
    EXPECT_ANY_THROW(seq.GetSubsequence(0, 3));
    EXPECT_ANY_THROW(seq.GetSubsequence(2, 1));

    auto* en = seq.GetEnumerator();
    while (en->MoveNext()) {
    }
    EXPECT_ANY_THROW(en->GetCurrent());
    delete en;
}

TEST(ThrowCases, ListSequenceEmpty) {
    MutableListSequence<int> seq;
    EXPECT_ANY_THROW(seq.GetFirst());
    EXPECT_ANY_THROW(seq.GetLast());
    EXPECT_ANY_THROW(seq.Get(0));
    EXPECT_ANY_THROW(seq.Get(-1));
    EXPECT_ANY_THROW(seq.InsertAt(10, 1));
    EXPECT_ANY_THROW(seq.GetSubsequence(0, 0));

    auto* en = seq.GetEnumerator();
    EXPECT_ANY_THROW(en->GetCurrent());
    en->MoveNext();
    EXPECT_ANY_THROW(en->GetCurrent());
    delete en;
}

TEST(ThrowCases, ListSequenceBounds) {
    int data[] = { 1, 2, 3 };
    MutableListSequence<int> seq(data, 3);

    EXPECT_ANY_THROW(seq.Get(3));
    EXPECT_ANY_THROW(seq.GetSubsequence(-1, 1));
    EXPECT_ANY_THROW(seq.GetSubsequence(0, 3));
    EXPECT_ANY_THROW(seq.GetSubsequence(2, 1));

    auto* en = seq.GetEnumerator();
    while (en->MoveNext()) {
    }
    EXPECT_ANY_THROW(en->GetCurrent());
    delete en;
}

TEST(ThrowCases, MapReduceNullArgs) {
    int data[] = { 1, 2, 3 };
    MutableArraySequence<int> seq(data, 3);

    auto mapNullSeq = [&] {
        Map<int, int>(
            nullptr,
            [](const int& x) { return x * 2; },
            []() -> Sequence<int>*{ return new MutableArraySequence<int>(); }
        );
    };
    EXPECT_ANY_THROW(mapNullSeq());

    auto mapNullFunction = [&] {
        Map<int, int>(
            &seq,
            std::function<int(const int&)>(),
            []() -> Sequence<int>*{ return new MutableArraySequence<int>(); }
        );
    };
    EXPECT_ANY_THROW(mapNullFunction());

    auto mapNullFactory = [&] {
        Map<int, int>(
            &seq,
            [](const int& x) { return x * 2; },
            std::function<Sequence<int>*()>()
        );
    };
    EXPECT_ANY_THROW(mapNullFactory());

    auto whereNullSeq = [&] {
        Where<int>(
            nullptr,
            [](const int& x) { return x > 0; },
            []() -> Sequence<int>*{ return new MutableArraySequence<int>(); }
        );
    };
    EXPECT_ANY_THROW(whereNullSeq());

    auto reduceNullSeq = [&] {
        Reduce<int, int>(
            nullptr,
            [](const int& acc, const int& x) { return acc + x; },
            0
        );
    };
    EXPECT_ANY_THROW(reduceNullSeq());
}

TEST(ThrowCases, ZipUnzipNullArgs) {
    int a[] = { 1, 2 };
    int b[] = { 10, 20 };

    MutableArraySequence<int> first(a, 2);
    MutableArraySequence<int> second(b, 2);

    auto zipFirstNull = [&] {
        Zip<int, int>(
            nullptr,
            &second,
            []() -> Sequence<Pair<int, int>>*{
                return new MutableArraySequence<Pair<int, int>>();
            }
        );
    };
    EXPECT_ANY_THROW(zipFirstNull());

    auto zipSecondNull = [&] {
        Zip<int, int>(
            &first,
            nullptr,
            []() -> Sequence<Pair<int, int>>*{
                return new MutableArraySequence<Pair<int, int>>();
            }
        );
    };
    EXPECT_ANY_THROW(zipSecondNull());

    auto unzipNull = [&] {
        Sequence<int>* outFirst = nullptr;
        Sequence<int>* outSecond = nullptr;

        Unzip<int, int>(
            nullptr,
            outFirst,
            outSecond,
            []() -> Sequence<int>* { return new MutableArraySequence<int>(); },
            []() -> Sequence<int>* { return new MutableArraySequence<int>(); }
        );
    };
    EXPECT_ANY_THROW(unzipNull());
}
