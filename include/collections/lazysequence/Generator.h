#pragma once

#include <memory>

#include "Ordinal.h"

template <class T>
class Generator {
public:
    virtual ~Generator() = default;

    virtual Ordinal GetLength() const = 0;
    virtual bool HasNext(Ordinal nextIndex) const = 0;
    virtual T Get(Ordinal nextIndex) = 0;
    virtual std::unique_ptr<Generator<T>> Clone() const = 0;
};
