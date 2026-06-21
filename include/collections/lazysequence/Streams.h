#pragma once

#include <cstddef>
#include <fstream>
#include <functional>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>

#include "LazySequence.h"
#include "Ordinal.h"
#include "../Sequence.h"
#include "../../utils/IEnumerator.h"

class EndOfStream : public std::out_of_range {
public:
    explicit EndOfStream(const std::string& message) : std::out_of_range(message) {}
};

class StreamException : public std::runtime_error {
public:
    explicit StreamException(const std::string& message) : std::runtime_error(message) {}
};

template <class T>
class ReadOnlyStream {
public:
    virtual ~ReadOnlyStream() = default;

    virtual void Open() = 0;
    virtual void Close() = 0;
    virtual bool IsEndOfStream() const = 0;
    virtual T Read() = 0;
    virtual std::size_t GetPosition() const = 0;
    virtual bool IsCanSeek() const = 0;
    virtual std::size_t Seek(std::size_t index) = 0;
    virtual bool IsCanGoBack() const = 0;
};

template <class T>
class WriteOnlyStream {
public:
    virtual ~WriteOnlyStream() = default;

    virtual void Open() = 0;
    virtual void Close() = 0;
    virtual std::size_t Write(const T& item) = 0;
    virtual std::size_t GetPosition() const = 0;
};

template <class T>
class SequenceReadStream : public ReadOnlyStream<T> {
private:
    std::shared_ptr<Sequence<T>> source_;
    std::unique_ptr<IEnumerator<T>> enumerator_;
    std::size_t position_;
    bool opened_;

    void EnsureOpened() const;

public:
    explicit SequenceReadStream(Sequence<T>& sourceSequence);
    explicit SequenceReadStream(std::shared_ptr<Sequence<T>> sourceSequence);

    void Open() override;
    void Close() override;
    bool IsEndOfStream() const override;
    T Read() override;
    std::size_t GetPosition() const override;
    bool IsCanSeek() const override;
    std::size_t Seek(std::size_t index) override;
    bool IsCanGoBack() const override;
};

template <class T>
class LazySequenceReadStream : public ReadOnlyStream<T> {
private:
    std::shared_ptr<LazySequence<T>> source_;
    std::size_t position_;
    bool opened_;

    void EnsureOpened() const;

public:
    explicit LazySequenceReadStream(LazySequence<T>& sourceSequence);
    explicit LazySequenceReadStream(std::shared_ptr<LazySequence<T>> sourceSequence);

    void Open() override;
    void Close() override;
    bool IsEndOfStream() const override;
    T Read() override;
    std::size_t GetPosition() const override;
    bool IsCanSeek() const override;
    std::size_t Seek(std::size_t index) override;
    bool IsCanGoBack() const override;
};

template <class T>
class StringReadStream : public ReadOnlyStream<T> {
private:
    std::string source_;
    std::function<T(const std::string&)> deserializer_;
    std::istringstream input_;
    std::size_t position_;
    bool opened_;
    bool endReached_;

    void EnsureOpened() const;

public:
    StringReadStream(std::string text, std::function<T(const std::string&)> itemDeserializer);

    void Open() override;
    void Close() override;
    bool IsEndOfStream() const override;
    T Read() override;
    std::size_t GetPosition() const override;
    bool IsCanSeek() const override;
    std::size_t Seek(std::size_t index) override;
    bool IsCanGoBack() const override;
};

template <class T>
class FileReadStream : public ReadOnlyStream<T> {
private:
    std::string filename_;
    std::function<T(const std::string&)> deserializer_;
    std::ifstream input_;
    std::size_t position_;
    bool opened_;
    bool endReached_;

    void EnsureOpened() const;

public:
    FileReadStream(std::string fileName, std::function<T(const std::string&)> itemDeserializer);

    void Open() override;
    void Close() override;
    bool IsEndOfStream() const override;
    T Read() override;
    std::size_t GetPosition() const override;
    bool IsCanSeek() const override;
    std::size_t Seek(std::size_t index) override;
    bool IsCanGoBack() const override;
};

template <class T>
class SequenceWriteStream : public WriteOnlyStream<T> {
private:
    std::shared_ptr<Sequence<T>> destination_;
    std::size_t position_;
    bool opened_;

    void EnsureOpened() const;

public:
    explicit SequenceWriteStream(Sequence<T>& destinationSequence);
    explicit SequenceWriteStream(std::shared_ptr<Sequence<T>> destinationSequence);

    void Open() override;
    void Close() override;
    std::size_t Write(const T& item) override;
    std::size_t GetPosition() const override;
};

template <class T>
class FileWriteStream : public WriteOnlyStream<T> {
private:
    std::string filename_;
    std::function<std::string(const T&)> serializer_;
    std::ofstream output_;
    std::size_t position_;
    bool opened_;
    bool appendMode_;

    void EnsureOpened() const;

public:
    FileWriteStream(std::string fileName, std::function<std::string(const T&)> itemSerializer,
                    bool useAppendMode = false);

    void Open() override;
    void Close() override;
    std::size_t Write(const T& item) override;
    std::size_t GetPosition() const override;
};

#include "details/Streams.tpp"
