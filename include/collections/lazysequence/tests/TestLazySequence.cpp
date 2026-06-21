#include <gtest/gtest.h>
#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include "../LazySequence.h"
#include "../../ArraySequence.h"

// --- Construction ---

TEST(LazySequence, EmptyByDefault) {
    LazySequence<int> s;
    EXPECT_EQ(s.GetLength().FinitePart(), 0u);
    EXPECT_TRUE(s.GetLength().IsFinite());
    EXPECT_THROW(s.GetFirst(), std::out_of_range);
    EXPECT_THROW(s.Get(Ordinal::Finite(0)), std::out_of_range);
}

TEST(LazySequence, FromArray) {
    int d[] = {10, 20, 30, 40};
    LazySequence<int> s(d, 4);
    EXPECT_EQ(s.GetLength().FinitePart(), 4u);
    EXPECT_EQ(s.GetFirst(), 10);
    EXPECT_EQ(s.GetLast(), 40);
    EXPECT_EQ(s.Get(Ordinal::Finite(2)), 30);
}

TEST(LazySequence, FromSequence) {
    int d[] = {1, 2, 3};
    MutableArraySequence<int> seq(d, 3);
    LazySequence<int> s(seq);
    EXPECT_EQ(s.GetLength().FinitePart(), 3u);
    EXPECT_EQ(s.Get(Ordinal::Finite(1)), 2);
}

TEST(LazySequence, FromNullSequencePointerThrows) {
    EXPECT_THROW(LazySequence<int>(static_cast<Sequence<int>*>(nullptr)),
                 std::invalid_argument);
}

TEST(LazySequence, MaterializationIsLazy) {
    int d[] = {1, 2, 3, 4};
    LazySequence<int> s(d, 4);
    EXPECT_EQ(s.GetMaterializedCount(), 0u);
    s.Get(Ordinal::Finite(2));
    EXPECT_EQ(s.GetMaterializedCount(), 3u);  // indexes 0,1,2
}

// --- Index function / infinite ---

TEST(LazySequence, FromIndexFunctionFinite) {
    auto f = LazySequence<int>::FromIndexFunction(
        [](std::size_t i) { return static_cast<int>(i) * 10; }, Ordinal::Finite(3));
    EXPECT_EQ(f->GetLength().FinitePart(), 3u);
    EXPECT_EQ(f->Get(Ordinal::Finite(2)), 20);
    EXPECT_THROW(f->Get(Ordinal::Finite(3)), std::out_of_range);
}

TEST(LazySequence, InfiniteSquares) {
    auto sq = LazySequence<int>::Infinite(
        [](std::size_t i) { return static_cast<int>(i * i); });
    EXPECT_TRUE(sq->GetLength().IsInfinite());
    EXPECT_EQ(sq->Get(Ordinal::Finite(0)), 0);
    EXPECT_EQ(sq->Get(Ordinal::Finite(5)), 25);
    EXPECT_THROW(sq->GetLast(), std::logic_error);  // omega length
}

// --- Recurrence ---

TEST(LazySequence, RecurrenceFibonacci) {
    MutableArraySequence<int> seed;
    seed.Append(0);
    seed.Append(1);
    LazySequence<int> fib(
        std::function<int(Sequence<int>*)>([](Sequence<int>* s) {
            int n = s->GetLength();
            return s->Get(n - 1) + s->Get(n - 2);
        }),
        &seed, Ordinal::Omega());

    EXPECT_TRUE(fib.GetLength().IsInfinite());
    int expected[] = {0, 1, 1, 2, 3, 5, 8, 13, 21, 34};
    for (std::size_t i = 0; i < 10; ++i)
        EXPECT_EQ(fib.Get(Ordinal::Finite(i)), expected[i]);
}

TEST(LazySequence, RecurrenceNullSeedThrows) {
    auto rule = std::function<int(Sequence<int>*)>([](Sequence<int>*) { return 0; });
    EXPECT_THROW(LazySequence<int>(rule, nullptr, Ordinal::Omega()), std::invalid_argument);
}

TEST(LazySequence, RecurrenceFiniteLengthLessThanSeedThrows) {
    MutableArraySequence<int> seed;
    seed.Append(0);
    seed.Append(1);
    seed.Append(2);
    auto rule = std::function<int(Sequence<int>*)>([](Sequence<int>*) { return 0; });
    EXPECT_THROW(LazySequence<int>(rule, &seed, Ordinal::Finite(2)), std::invalid_argument);
}

// --- Append / Prepend / Concat / Insert ---

TEST(LazySequence, AppendElement) {
    int d[] = {1, 2, 3};
    LazySequence<int> s(d, 3);
    auto r = s.Append(99);
    EXPECT_EQ(r->GetLength().FinitePart(), 4u);
    EXPECT_EQ(r->Get(Ordinal::Finite(3)), 99);
}

TEST(LazySequence, PrependElement) {
    int d[] = {1, 2, 3};
    LazySequence<int> s(d, 3);
    auto r = s.Prepend(0);
    EXPECT_EQ(r->GetLength().FinitePart(), 4u);
    EXPECT_EQ(r->Get(Ordinal::Finite(0)), 0);
    EXPECT_EQ(r->Get(Ordinal::Finite(1)), 1);
}

TEST(LazySequence, AppendSequence) {
    int a[] = {1, 2, 3};
    int b[] = {4, 5};
    LazySequence<int> la(a, 3);
    MutableArraySequence<int> sb(b, 2);
    auto r = la.Append(static_cast<const Sequence<int>&>(sb));
    EXPECT_EQ(r->GetLength().FinitePart(), 5u);

    for (int i = 0; i < 5; i++) {
        EXPECT_EQ(r->Get(Ordinal::Finite(i)), i+1);
    }
}

TEST(LazySequence, AppendLazySequence) {
    int a[] = {1, 2};
    int b[] = {3, 4};
    LazySequence<int> la(a, 2), lb(b, 2);
    auto r = la.Append(lb);
    EXPECT_EQ(r->GetLength().FinitePart(), 4u);
    EXPECT_EQ(r->Get(Ordinal::Finite(2)), 3);
}

TEST(LazySequence, Concat) {
    int a[] = {1, 2, 3};
    int b[] = {4, 5};
    LazySequence<int> la(a, 3), lb(b, 2);
    auto r = la.Concat(lb);
    EXPECT_EQ(r->GetLength().FinitePart(), 5u);
    EXPECT_EQ(r->Get(Ordinal::Finite(0)), 1);
    EXPECT_EQ(r->Get(Ordinal::Finite(4)), 5);
}

TEST(LazySequence, ConcatTwoInfinite) {
    // first = 0,1,2,...   second = 100,101,102,...
    auto first = LazySequence<int>::Infinite(
        [](std::size_t i) { return static_cast<int>(i); });
    auto second = LazySequence<int>::Infinite(
        [](std::size_t i) { return static_cast<int>(100 + i); });
    auto r = first->Concat(*second);

    // длина = ω + ω = 2ω
    EXPECT_TRUE(r->GetLength().IsInfinite());
    EXPECT_EQ(r->GetLength().OmegaCoefficient(), 2u);
    EXPECT_EQ(r->GetLength().FinitePart(), 0u);

    // любой КОНЕЧНЫЙ индекс попадает только в первую последовательность
    EXPECT_EQ(r->Get(Ordinal::Finite(0)), 0);
    EXPECT_EQ(r->Get(Ordinal::Finite(5)), 5);
    EXPECT_EQ(r->Get(Ordinal::Finite(1000)), 1000);  // не 1100 — вторая недостижима конечным индексом

    // вторая последовательность доступна только ТРАНСФИНИТНЫМИ индексами ω + k
    EXPECT_EQ(r->Get(Ordinal(1, 0)), 100);   // ω      -> second[0]
    EXPECT_EQ(r->Get(Ordinal(1, 3)), 103);   // ω + 3  -> second[3]
}

TEST(LazySequence, ConcatNullThrows) {
    int a[] = {1};
    LazySequence<int> la(a, 1);
    EXPECT_THROW(la.Concat(static_cast<const LazySequence<int>*>(nullptr)),
                 std::invalid_argument);
}

TEST(LazySequence, InsertAt) {
    int d[] = {1, 2, 3};
    LazySequence<int> s(d, 3);
    auto r = s.InsertAt(99, static_cast<std::size_t>(1));
    EXPECT_EQ(r->GetLength().FinitePart(), 4u);
    EXPECT_EQ(r->Get(Ordinal::Finite(0)), 1);
    EXPECT_EQ(r->Get(Ordinal::Finite(1)), 99);
    EXPECT_EQ(r->Get(Ordinal::Finite(2)), 2);
}

TEST(LazySequence, InsertAtNegativeThrows) {
    int d[] = {1, 2, 3};
    LazySequence<int> s(d, 3);
    EXPECT_THROW(s.InsertAt(5, -1), std::out_of_range);
}

TEST(LazySequence, InsertAtOutOfRangeThrows) {
    int d[] = {1, 2, 3};
    LazySequence<int> s(d, 3);
    EXPECT_THROW(s.InsertAt(5, static_cast<std::size_t>(3)), std::out_of_range);
}

// --- Map / Where ---

TEST(LazySequence, Map) {
    int d[] = {1, 2, 3};
    LazySequence<int> s(d, 3);
    auto m = s.Map<int>([](int x) { return x * x; });
    EXPECT_EQ(m->GetLength().FinitePart(), 3u);
    EXPECT_EQ(m->Get(Ordinal::Finite(2)), 9);
}

TEST(LazySequence, MapChangesType) {
    int d[] = {1, 2, 3};
    LazySequence<int> s(d, 3);
    auto m = s.Map<std::string>([](int x) { return std::to_string(x); });
    EXPECT_EQ(m->Get(Ordinal::Finite(0)), "1");
}

TEST(LazySequence, MapEmptyFunctionThrows) {
    int d[] = {1};
    LazySequence<int> s(d, 1);
    std::function<int(int)> empty;
    EXPECT_THROW(s.Map<int>(empty), std::invalid_argument);
}

TEST(LazySequence, WhereFinite) {
    int d[] = {1, 2, 3, 4, 5, 6};
    LazySequence<int> s(d, 6);
    auto w = s.Where([](int x) { return x % 2 == 0; });
    EXPECT_EQ(w->GetLength().FinitePart(), 3u);
    EXPECT_EQ(w->Get(Ordinal::Finite(0)), 2);
    EXPECT_EQ(w->Get(Ordinal::Finite(2)), 6);
}

TEST(LazySequence, WhereInfinite) {
    auto nats = LazySequence<int>::Infinite(
        [](std::size_t i) { return static_cast<int>(i); });
    auto odd = nats->Where([](int x) { return x % 2 != 0; });
    EXPECT_EQ(odd->Get(Ordinal::Finite(0)), 1);
    EXPECT_EQ(odd->Get(Ordinal::Finite(3)), 7);
}

TEST(LazySequence, WhereEmptyPredicateThrows) {
    int d[] = {1};
    LazySequence<int> s(d, 1);
    std::function<bool(int)> empty;
    EXPECT_THROW(s.Where(empty), std::invalid_argument);
}

// --- Subsequence ---

TEST(LazySequence, GetSubsequenceFinite) {
    int d[] = {10, 20, 30, 40};
    LazySequence<int> s(d, 4);
    auto sub = s.GetSubsequence(static_cast<std::size_t>(1), static_cast<std::size_t>(2));
    EXPECT_EQ(sub->GetLength().FinitePart(), 2u);  // inclusive end
    EXPECT_EQ(sub->Get(Ordinal::Finite(0)), 20);
    EXPECT_EQ(sub->Get(Ordinal::Finite(1)), 30);
}

TEST(LazySequence, GetSubsequenceWholeRange) {
    int d[] = {10, 20, 30, 40};
    LazySequence<int> s(d, 4);
    auto sub = s.GetSubsequence(static_cast<std::size_t>(0), static_cast<std::size_t>(3));
    EXPECT_EQ(sub->GetLength().FinitePart(), 4u);
}

TEST(LazySequence, GetSubsequenceOutOfRangeThrows) {
    int d[] = {10, 20, 30, 40};
    LazySequence<int> s(d, 4);
    EXPECT_THROW(s.GetSubsequence(static_cast<std::size_t>(0), static_cast<std::size_t>(4)),
                 std::out_of_range);
    EXPECT_THROW(s.GetSubsequence(static_cast<std::size_t>(2), static_cast<std::size_t>(1)),
                 std::out_of_range);
}

TEST(LazySequence, GetSubsequenceNegativeThrows) {
    int d[] = {10, 20, 30, 40};
    LazySequence<int> s(d, 4);
    EXPECT_THROW(s.GetSubsequence(-1, 2), std::out_of_range);
}

TEST(LazySequence, GetSubsequenceInfiniteTail) {
    auto nats = LazySequence<int>::Infinite(
        [](std::size_t i) { return static_cast<int>(i); });
    auto tail = nats->GetSubsequence(Ordinal::Finite(2), Ordinal::Omega());
    EXPECT_TRUE(tail->GetLength().IsInfinite());
    EXPECT_EQ(tail->Get(Ordinal::Finite(0)), 2);
    EXPECT_EQ(tail->Get(Ordinal::Finite(5)), 7);
}

// --- Reduce / Take ---

TEST(LazySequence, ReduceFinite) {
    int d[] = {1, 2, 3, 4};
    LazySequence<int> s(d, 4);
    EXPECT_EQ(s.Reduce<int>(0, [](int a, int b) { return a + b; }), 10);
}

TEST(LazySequence, ReduceFirstN) {
    int d[] = {1, 2, 3, 4};
    LazySequence<int> s(d, 4);
    EXPECT_EQ(s.ReduceFirstN<int>(2, 0, [](int a, int b) { return a + b; }), 3);
}

TEST(LazySequence, ReduceInfiniteThrows) {
    auto nats = LazySequence<int>::Infinite(
        [](std::size_t i) { return static_cast<int>(i); });
    EXPECT_THROW(nats->Reduce<long long>(0, [](long long a, int b) { return a + b; }),
                 std::logic_error);
}

TEST(LazySequence, ReduceFirstNOnInfinite) {
    auto nats = LazySequence<int>::Infinite(
        [](std::size_t i) { return static_cast<int>(i); });
    long long s = nats->ReduceFirstN<long long>(
        5, 0LL, [](long long a, int b) { return a + b; });
    EXPECT_EQ(s, 10);  // 0+1+2+3+4
}

TEST(LazySequence, Take) {
    auto nats = LazySequence<int>::Infinite(
        [](std::size_t i) { return static_cast<int>(i); });
    std::unique_ptr<Sequence<int>> taken = nats->Take(4);
    EXPECT_EQ(taken->GetLength(), 4);
    EXPECT_EQ(taken->Get(0), 0);
    EXPECT_EQ(taken->Get(3), 3);
}

// --- Enumerator ---

TEST(LazySequence, EnumeratorIterates) {
    int d[] = {5, 6, 7};
    LazySequence<int> s(d, 3);
    auto en = s.GetEnumerator();

    int sum = 0;
    int count = 0;
    while (en->MoveNext()) {
        sum += en->GetCurrent();
        ++count;
    }
    EXPECT_EQ(count, 3);
    EXPECT_EQ(sum, 18);
}

TEST(LazySequence, EnumeratorResetAndCurrentBeforeMove) {
    int d[] = {5, 6, 7};
    LazySequence<int> s(d, 3);
    auto en = s.GetEnumerator();

    EXPECT_THROW(en->GetCurrent(), std::out_of_range);  // before first MoveNext
    en->MoveNext();
    EXPECT_EQ(en->GetCurrent(), 5);
    en->Reset();
    EXPECT_THROW(en->GetCurrent(), std::out_of_range);
    en->MoveNext();
    EXPECT_EQ(en->GetCurrent(), 5);
}

// --- GetLast edge cases ---

TEST(LazySequence, GetLastEmptyThrows) {
    LazySequence<int> s;
    EXPECT_THROW(s.GetLast(), std::out_of_range);
}

// --- Copy / assignment ---

TEST(LazySequence, CopyConstructor) {
    int d[] = {1, 2, 3};
    LazySequence<int> s(d, 3);
    LazySequence<int> copy(s);
    EXPECT_EQ(copy.GetLength().FinitePart(), 3u);
    EXPECT_EQ(copy.Get(Ordinal::Finite(1)), 2);
}

TEST(LazySequence, CopyAssignment) {
    int d[] = {1, 2, 3};
    LazySequence<int> s(d, 3);
    LazySequence<int> other;
    other = s;
    EXPECT_EQ(other.GetLength().FinitePart(), 3u);
    EXPECT_EQ(other.Get(Ordinal::Finite(2)), 3);
}

// --- Chained lazy operations ---

TEST(LazySequence, ChainMapWhereOnInfinite) {
    auto nats = LazySequence<int>::Infinite(
        [](std::size_t i) { return static_cast<int>(i); });
    auto mapped = nats->Map<int>([](int x) { return x + 1; });      // 1,2,3,...
    auto evens = mapped->Where([](int x) { return x % 2 == 0; });   // 2,4,6,...
    EXPECT_EQ(evens->Get(Ordinal::Finite(0)), 2);
    EXPECT_EQ(evens->Get(Ordinal::Finite(3)), 8);
}
