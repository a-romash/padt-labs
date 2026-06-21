#pragma once

#include <cstddef>
#include <string>

class Ordinal {
private:
    std::size_t omegaCoefficient_;
    std::size_t finitePart_;

public:
    Ordinal();
    Ordinal(std::size_t omegaCoefficient, std::size_t finitePart);

    static Ordinal Finite(std::size_t value);
    static Ordinal Omega();
    static Ordinal Omega(std::size_t omegaCoefficient);
    static Ordinal FromParts(std::size_t omegaCoefficient, std::size_t finitePart);

    bool IsFinite() const;
    bool IsOmega() const;
    bool IsInfinite() const;

    std::size_t OmegaCoefficient() const;
    std::size_t FinitePart() const;

    std::string ToString() const;

    Ordinal operator+(const Ordinal& other) const;

    bool operator==(const Ordinal& other) const;
    bool operator!=(const Ordinal& other) const;
    bool operator<(const Ordinal& other) const;
    bool operator<=(const Ordinal& other) const;
    bool operator>(const Ordinal& other) const;
    bool operator>=(const Ordinal& other) const;
};

#include "details/Ordinal.tpp"
