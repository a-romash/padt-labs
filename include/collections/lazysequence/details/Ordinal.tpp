#include <limits>
#include <stdexcept>

#include "../Ordinal.h"

inline Ordinal::Ordinal()
    : omegaCoefficient_(0), finitePart_(0) {
}

inline Ordinal::Ordinal(const std::size_t omegaCoefficient, const std::size_t finitePart)
    : omegaCoefficient_(omegaCoefficient), finitePart_(finitePart) {
}

inline Ordinal Ordinal::Finite(const std::size_t value) {
    return Ordinal(0, value);
}

inline Ordinal Ordinal::Omega() {
    return Ordinal(1, 0);
}

inline Ordinal Ordinal::Omega(const std::size_t omegaCoefficient) {
    return Ordinal(omegaCoefficient, 0);
}

inline Ordinal Ordinal::FromParts(const std::size_t omegaCoefficient, const std::size_t finitePart) {
    return Ordinal(omegaCoefficient, finitePart);
}

inline bool Ordinal::IsFinite() const {
    return omegaCoefficient_ == 0;
}

inline bool Ordinal::IsOmega() const {
    return omegaCoefficient_ == 1 && finitePart_ == 0;
}

inline bool Ordinal::IsInfinite() const {
    return omegaCoefficient_ > 0;
}

inline std::size_t Ordinal::OmegaCoefficient() const {
    return omegaCoefficient_;
}

inline std::size_t Ordinal::FinitePart() const {
    return finitePart_;
}

inline std::string Ordinal::ToString() const {
    if (IsFinite())
        return std::to_string(finitePart_);
    if (omegaCoefficient_ == 1 && finitePart_ == 0)
        return "omega";

    std::string result = "omega";
    if (omegaCoefficient_ > 1)
        result += " * " + std::to_string(omegaCoefficient_);
    if (finitePart_ > 0)
        result += " + " + std::to_string(finitePart_);
    return result;
}

inline Ordinal Ordinal::operator+(const Ordinal& other) const {
    if (other.omegaCoefficient_ > 0) {
        if (omegaCoefficient_ > std::numeric_limits<std::size_t>::max() - other.omegaCoefficient_)
            throw std::overflow_error("Ordinal omega coefficient addition overflow");
        return Ordinal(omegaCoefficient_ + other.omegaCoefficient_, other.finitePart_);
    }
    if (finitePart_ > std::numeric_limits<std::size_t>::max() - other.finitePart_)
        throw std::overflow_error("Ordinal finite part addition overflow");
    return Ordinal(omegaCoefficient_, finitePart_ + other.finitePart_);
}

inline bool Ordinal::operator==(const Ordinal& other) const {
    return omegaCoefficient_ == other.omegaCoefficient_ && finitePart_ == other.finitePart_;
}

inline bool Ordinal::operator!=(const Ordinal& other) const {
    return !(*this == other);
}

inline bool Ordinal::operator<(const Ordinal& other) const {
    if (omegaCoefficient_ != other.omegaCoefficient_)
        return omegaCoefficient_ < other.omegaCoefficient_;
    return finitePart_ < other.finitePart_;
}

inline bool Ordinal::operator<=(const Ordinal& other) const {
    return *this < other || *this == other;
}

inline bool Ordinal::operator>(const Ordinal& other) const {
    return !(*this <= other);
}

inline bool Ordinal::operator>=(const Ordinal& other) const {
    return !(*this < other);
}
