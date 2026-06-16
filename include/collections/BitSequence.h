#pragma once

class BitSequence {
private:
    char* data_;
    int length_; // количество бит
    int byteCount_; // количество байт = (length + 7) / 8

    void CheckIndex(int index) const;
    void CheckSameLength(const BitSequence& other) const;

public:
    BitSequence();
    BitSequence(int size);
    BitSequence(const char* rawData, int size);
    BitSequence(const BitSequence& other);
    BitSequence& operator=(const BitSequence& other);
    ~BitSequence();

    int GetLength() const;

    int Get(int index) const; // возвращает 0 или 1
    void Set(int index, int bit); // устанавливает 0 или 1
    void Flip(int index); // инвертирует бит

    //без создания нового элементы внутри
    void AND(const BitSequence& other, BitSequence& result) const;
    void OR(const BitSequence& other, BitSequence& result) const;
    void XOR(const BitSequence& other, BitSequence& result) const;
    void NOT(BitSequence& result) const;

    void Print() const;

    int operator[](int index) const;
    bool operator==(const BitSequence& other) const;


    BitSequence operator&(const BitSequence& other) const;
    BitSequence operator|(const BitSequence& other) const;
    BitSequence operator^(const BitSequence& other) const;
    BitSequence operator~() const;
};

#include "details/BitSequence.tpp"
