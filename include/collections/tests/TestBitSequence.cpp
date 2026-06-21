#include <gtest/gtest.h>
#include "../BitSequence.h"

TEST(BitSequence, DefaultLengthZero) {
    BitSequence bs;
    EXPECT_EQ(bs.GetLength(), 0);
}

TEST(BitSequence, LengthEightAllZero) {
    BitSequence bs(8);
    EXPECT_EQ(bs.GetLength(), 8);
    bool allZero = true;
    for (int i = 0; i < 8; ++i) if (bs.Get(i) != 0) allZero = false;
    EXPECT_TRUE(allZero);
}

TEST(BitSequence, SetBits) {
    BitSequence bs(8); bs.Set(0, 1); bs.Set(3, 1);
    EXPECT_EQ(bs.Get(0), 1);
    EXPECT_EQ(bs.Get(1), 0);
    EXPECT_EQ(bs.Get(3), 1);
}

TEST(BitSequence, Flip) {
    BitSequence bs(4); bs.Set(1, 1); bs.Flip(1);
    EXPECT_EQ(bs.Get(1), 0);
    bs.Flip(1);
    EXPECT_EQ(bs.Get(1), 1);
}

TEST(BitSequence, CopyConstructorIndependent) {
    BitSequence a(4); a.Set(0, 1); BitSequence b(a); b.Set(0, 0);
    EXPECT_EQ(a.Get(0), 1);
}

TEST(BitSequence, AssignmentIndependent) {
    BitSequence a(4); a.Set(2, 1); BitSequence b(4); b = a; b.Set(2, 0);
    EXPECT_EQ(a.Get(2), 1);
}

TEST(BitSequence, Indexing) {
    BitSequence bs(4); bs.Set(2, 1);
    EXPECT_EQ(bs[2], 1);
}

TEST(BitSequence, And) {
    BitSequence a(4), b(4); a.Set(0, 1); a.Set(1, 1); b.Set(1, 1); b.Set(2, 1);
    BitSequence r(4);
    a.AND(b, r);
    EXPECT_EQ(r.Get(0), 0);
    EXPECT_EQ(r.Get(1), 1);
    EXPECT_EQ(r.Get(2), 0);
}

TEST(BitSequence, Or) {
    BitSequence a(4), b(4); a.Set(0, 1); b.Set(1, 1);
    BitSequence r(4);
    a.OR(b, r);
    EXPECT_EQ(r.Get(0), 1);
    EXPECT_EQ(r.Get(1), 1);
    EXPECT_EQ(r.Get(2), 0);
}

TEST(BitSequence, Xor) {
    BitSequence a(4), b(4); a.Set(0, 1); a.Set(1, 1); b.Set(1, 1); b.Set(2, 1);
    BitSequence r(4);
    a.XOR(b, r);
    EXPECT_EQ(r.Get(0), 1);
    EXPECT_EQ(r.Get(1), 0);
    EXPECT_EQ(r.Get(2), 1);
}

TEST(BitSequence, Not) {
    BitSequence a(4); a.Set(0, 1); a.Set(2, 1);
    BitSequence r(4);
    a.NOT(r);
    EXPECT_EQ(r.Get(0), 0);
    EXPECT_EQ(r.Get(1), 1);
    EXPECT_EQ(r.Get(2), 0);
    EXPECT_EQ(r.Get(3), 1);
}

TEST(BitSequence, OperatorAnd) {
    BitSequence a(4), b(4); a.Set(0, 1); b.Set(0, 1);
    auto r = a & b;
    EXPECT_EQ(r.Get(0), 1);
}

TEST(BitSequence, OperatorOr) {
    BitSequence a(4), b(4); a.Set(0, 1); b.Set(1, 1);
    auto r = a | b;
    EXPECT_EQ(r.Get(0), 1);
    EXPECT_EQ(r.Get(1), 1);
}

TEST(BitSequence, OperatorXor) {
    BitSequence a(4), b(4); a.Set(0, 1); b.Set(0, 1);
    auto r = a ^ b;
    EXPECT_EQ(r.Get(0), 0);
}

TEST(BitSequence, OperatorNot) {
    BitSequence a(4); a.Set(0, 1); auto r = ~a;
    EXPECT_EQ(r.Get(0), 0);
    EXPECT_EQ(r.Get(1), 1);
}

TEST(BitSequence, Bitmask) {
    BitSequence data(4), mask(4);
    data.Set(0, 1); data.Set(1, 1); data.Set(2, 1); data.Set(3, 1);
    mask.Set(0, 1); mask.Set(2, 1);
    auto r = data & mask;
    EXPECT_EQ(r.Get(0), 1);
    EXPECT_EQ(r.Get(1), 0);
    EXPECT_EQ(r.Get(2), 1);
    EXPECT_EQ(r.Get(3), 0);
}

TEST(BitSequence, NotLengthNineAllOnes) {
    BitSequence a(9);
    BitSequence r(9);
    a.NOT(r);
    EXPECT_EQ(r.GetLength(), 9);
    bool allOne = true;
    for (int i = 0; i < 9; ++i) if (r.Get(i) != 1) allOne = false;
    EXPECT_TRUE(allOne);
}

TEST(BitSequence, SingleBit) {
    BitSequence bs(1); bs.Set(0, 1);
    EXPECT_EQ(bs.Get(0), 1);
    bs.Flip(0);
    EXPECT_EQ(bs.Get(0), 0);
}

TEST(BitSequence, LengthThirteen) {
    BitSequence bs(13);
    EXPECT_EQ(bs.GetLength(), 13);
    bs.Set(12, 1);
    EXPECT_EQ(bs.Get(12), 1);
}

TEST(BitSequence, NegativeSizeThrows) {
    EXPECT_ANY_THROW(BitSequence(-1));
}

TEST(BitSequence, GetOutOfRange) {
    BitSequence bs(4);
    EXPECT_ANY_THROW(bs.Get(-1));
    EXPECT_ANY_THROW(bs.Get(4));
}

TEST(BitSequence, SetOutOfRange) {
    BitSequence bs(4);
    EXPECT_ANY_THROW(bs.Set(-1, 1));
    EXPECT_ANY_THROW(bs.Set(4, 1));
}

TEST(BitSequence, SetBadValue) {
    BitSequence bs(4);
    EXPECT_ANY_THROW(bs.Set(0, 2));
    EXPECT_ANY_THROW(bs.Set(0, -1));
}

TEST(BitSequence, FlipOutOfRange) {
    BitSequence bs(4);
    EXPECT_ANY_THROW(bs.Flip(10));
}

TEST(BitSequence, SizeMismatchThrows) {
    BitSequence a(4), b(8), r(4);
    EXPECT_ANY_THROW(a.AND(b, r));
    EXPECT_ANY_THROW(a.OR(b, r));
    EXPECT_ANY_THROW(a.XOR(b, r));
}

// Перенесено из секции Throw cases (BitSequence — конкретный класс,
// его заголовок подключается только в этом translation unit).
TEST(ThrowCases, BitSequenceBounds) {
    BitSequence bits(4);
    EXPECT_ANY_THROW(bits.Get(-1));
    EXPECT_ANY_THROW(bits.Get(4));
    EXPECT_ANY_THROW(bits.Set(-1, 1));
    EXPECT_ANY_THROW(bits.Set(4, 1));
    EXPECT_ANY_THROW(bits.Set(0, 2));
    EXPECT_ANY_THROW(bits.Set(0, -1));
    EXPECT_ANY_THROW(bits.Flip(4));
}

TEST(ThrowCases, BitSequenceSizeMismatch) {
    BitSequence a(3);
    BitSequence b(4);
    BitSequence result(3);
    EXPECT_ANY_THROW(a.AND(b, result));
    EXPECT_ANY_THROW(a.OR(b, result));
    EXPECT_ANY_THROW(a.XOR(b, result));
}
