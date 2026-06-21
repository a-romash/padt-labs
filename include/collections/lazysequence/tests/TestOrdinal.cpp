#include <gtest/gtest.h>
#include <limits>
#include <stdexcept>
#include "../Ordinal.h"

// --- Construction / classification ---

TEST(Ordinal, DefaultIsZero) {
    Ordinal o;
    EXPECT_TRUE(o.IsFinite());
    EXPECT_FALSE(o.IsInfinite());
    EXPECT_EQ(o.FinitePart(), 0u);
    EXPECT_EQ(o.OmegaCoefficient(), 0u);
}

TEST(Ordinal, Finite) {
    Ordinal o = Ordinal::Finite(42);
    EXPECT_TRUE(o.IsFinite());
    EXPECT_FALSE(o.IsOmega());
    EXPECT_EQ(o.FinitePart(), 42u);
}

TEST(Ordinal, Omega) {
    Ordinal o = Ordinal::Omega();
    EXPECT_TRUE(o.IsInfinite());
    EXPECT_TRUE(o.IsOmega());
    EXPECT_FALSE(o.IsFinite());
    EXPECT_EQ(o.OmegaCoefficient(), 1u);
}

TEST(Ordinal, OmegaWithCoefficient) {
    Ordinal o = Ordinal::Omega(3);
    EXPECT_TRUE(o.IsInfinite());
    EXPECT_FALSE(o.IsOmega());
    EXPECT_EQ(o.OmegaCoefficient(), 3u);
}

TEST(Ordinal, FromParts) {
    Ordinal o = Ordinal::FromParts(2, 5);
    EXPECT_EQ(o.OmegaCoefficient(), 2u);
    EXPECT_EQ(o.FinitePart(), 5u);
}

// --- Comparison ---

TEST(Ordinal, FiniteComparison) {
    EXPECT_TRUE(Ordinal::Finite(3) < Ordinal::Finite(5));
    EXPECT_TRUE(Ordinal::Finite(5) > Ordinal::Finite(3));
    EXPECT_TRUE(Ordinal::Finite(5) == Ordinal::Finite(5));
    EXPECT_TRUE(Ordinal::Finite(5) != Ordinal::Finite(6));
}

TEST(Ordinal, FiniteLessThanInfinite) {
    EXPECT_TRUE(Ordinal::Finite(1000000) < Ordinal::Omega());
    EXPECT_FALSE(Ordinal::Omega() < Ordinal::Finite(1000000));
}

TEST(Ordinal, InfiniteComparison) {
    EXPECT_TRUE(Ordinal::Omega() < Ordinal::FromParts(1, 1));   // omega < omega + 1
    EXPECT_TRUE(Ordinal::Omega() < Ordinal::Omega(2));          // omega < omega*2
    EXPECT_TRUE(Ordinal::Omega() == Ordinal::Omega());
    EXPECT_TRUE(Ordinal::Omega() != Ordinal::Finite(5));
}

TEST(Ordinal, LessEqualGreaterEqual) {
    EXPECT_TRUE(Ordinal::Finite(3) <= Ordinal::Finite(3));
    EXPECT_TRUE(Ordinal::Finite(3) >= Ordinal::Finite(3));
    EXPECT_TRUE(Ordinal::Finite(4) >= Ordinal::Finite(3));
    EXPECT_FALSE(Ordinal::Finite(2) >= Ordinal::Finite(3));
}

// --- Ordinal addition ---

TEST(Ordinal, FinitePlusFinite) {
    EXPECT_TRUE(Ordinal::Finite(2) + Ordinal::Finite(3) == Ordinal::Finite(5));
}

TEST(Ordinal, OmegaPlusFinite) {
    Ordinal r = Ordinal::Omega() + Ordinal::Finite(3);
    EXPECT_EQ(r.OmegaCoefficient(), 1u);
    EXPECT_EQ(r.FinitePart(), 3u);
}

TEST(Ordinal, FinitePlusOmegaIsAbsorbed) {
    // 2 + omega == omega  (left finite part is absorbed)
    Ordinal r = Ordinal::Finite(2) + Ordinal::Omega();
    EXPECT_TRUE(r == Ordinal::Omega());
}

TEST(Ordinal, OmegaPlusOmega) {
    EXPECT_TRUE(Ordinal::Omega() + Ordinal::Omega() == Ordinal::Omega(2));
}

TEST(Ordinal, AdditionOverflowThrows) {
    Ordinal big = Ordinal::Finite(std::numeric_limits<std::size_t>::max());
    EXPECT_THROW(big + Ordinal::Finite(1), std::overflow_error);

    Ordinal bigOmega = Ordinal::FromParts(std::numeric_limits<std::size_t>::max(), 0);
    EXPECT_THROW(bigOmega + Ordinal::Omega(), std::overflow_error);
}

// --- ToString ---

TEST(Ordinal, ToString) {
    EXPECT_EQ(Ordinal::Finite(0).ToString(), "0");
    EXPECT_EQ(Ordinal::Finite(42).ToString(), "42");
    EXPECT_EQ(Ordinal::Omega().ToString(), "omega");
    EXPECT_EQ(Ordinal::FromParts(1, 3).ToString(), "omega + 3");
    EXPECT_EQ(Ordinal::FromParts(2, 0).ToString(), "omega * 2");
    EXPECT_EQ(Ordinal::FromParts(2, 3).ToString(), "omega * 2 + 3");
}
