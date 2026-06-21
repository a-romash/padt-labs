#include <ios>
#include <stdexcept>
#include <utility>

#include "../Streams.h"

template <class T>
SequenceReadStream<T>::SequenceReadStream(Sequence<T>& sourceSequence)
    : source_(std::shared_ptr<Sequence<T>>(&sourceSequence, [](auto) {})),
      enumerator_(nullptr), position_(0), opened_(false) {
}

template <class T>
SequenceReadStream<T>::SequenceReadStream(std::shared_ptr<Sequence<T>> sourceSequence)
    : source_(std::move(sourceSequence)), enumerator_(nullptr), position_(0), opened_(false) {
    if (!source_)
        throw std::invalid_argument("SequenceReadStream source is null");
}

template <class T>
void SequenceReadStream<T>::Open() {
    enumerator_ = std::unique_ptr<IEnumerator<T>>(source_->GetEnumerator());
    position_ = 0;
    opened_ = true;
}

template <class T>
void SequenceReadStream<T>::Close() {
    enumerator_.reset();
    position_ = 0;
    opened_ = false;
}

template <class T>
bool SequenceReadStream<T>::IsEndOfStream() const {
    return position_ >= static_cast<std::size_t>(source_->GetLength());
}

template <class T>
T SequenceReadStream<T>::Read() {
    EnsureOpened();
    if (IsEndOfStream() || !enumerator_->MoveNext())
        throw EndOfStream("SequenceReadStream reached end");
    ++position_;
    return enumerator_->GetCurrent();
}

template <class T>
std::size_t SequenceReadStream<T>::GetPosition() const {
    return position_;
}

template <class T>
bool SequenceReadStream<T>::IsCanSeek() const {
    return true;
}

template <class T>
std::size_t SequenceReadStream<T>::Seek(std::size_t index) {
    EnsureOpened();
    if (index > static_cast<std::size_t>(source_->GetLength()))
        throw EndOfStream("Seek index is out of range");

    enumerator_ = std::unique_ptr<IEnumerator<T>>(source_->GetEnumerator());
    position_ = 0;
    while (position_ < index) {
        if (!enumerator_->MoveNext())
            throw EndOfStream("Seek index is out of range");
        ++position_;
    }
    return position_;
}

template <class T>
bool SequenceReadStream<T>::IsCanGoBack() const {
    return true;
}

template <class T>
void SequenceReadStream<T>::EnsureOpened() const {
    if (!opened_)
        throw StreamException("Stream is not opened");
}


template <class T>
LazySequenceReadStream<T>::LazySequenceReadStream(LazySequence<T>& sourceSequence)
    : source_(std::shared_ptr<LazySequence<T>>(&sourceSequence, [](auto) {})),
      position_(0), opened_(false) {
}

template <class T>
LazySequenceReadStream<T>::LazySequenceReadStream(std::shared_ptr<LazySequence<T>> sourceSequence)
    : source_(std::move(sourceSequence)), position_(0), opened_(false) {
    if (!source_)
        throw std::invalid_argument("LazySequenceReadStream source is null");
}

template <class T>
void LazySequenceReadStream<T>::Open() {
    position_ = 0;
    opened_ = true;
}

template <class T>
void LazySequenceReadStream<T>::Close() {
    position_ = 0;
    opened_ = false;
}

template <class T>
bool LazySequenceReadStream<T>::IsEndOfStream() const {
    Ordinal length = source_->GetLength();
    return length.IsFinite() && position_ >= length.FinitePart();
}

template <class T>
T LazySequenceReadStream<T>::Read() {
    EnsureOpened();
    if (IsEndOfStream())
        throw EndOfStream("LazySequenceReadStream reached end");
    T value = source_->Get(Ordinal::Finite(position_));
    ++position_;
    return value;
}

template <class T>
std::size_t LazySequenceReadStream<T>::GetPosition() const {
    return position_;
}

template <class T>
bool LazySequenceReadStream<T>::IsCanSeek() const {
    return true;
}

template <class T>
std::size_t LazySequenceReadStream<T>::Seek(std::size_t index) {
    EnsureOpened();
    Ordinal length = source_->GetLength();
    if (length.IsFinite() && index > length.FinitePart())
        throw EndOfStream("Seek index is out of range");
    position_ = index;
    return position_;
}

template <class T>
bool LazySequenceReadStream<T>::IsCanGoBack() const {
    return true;
}

template <class T>
void LazySequenceReadStream<T>::EnsureOpened() const {
    if (!opened_)
        throw StreamException("Stream is not opened");
}


template <class T>
StringReadStream<T>::StringReadStream(std::string text, std::function<T(const std::string&)> itemDeserializer)
    : source_(std::move(text)), deserializer_(std::move(itemDeserializer)), input_(), position_(0),
      opened_(false), endReached_(false) {
    if (!deserializer_)
        throw std::invalid_argument("Deserializer is empty");
}

template <class T>
void StringReadStream<T>::Open() {
    input_.clear();
    input_.str(source_);
    position_ = 0;
    opened_ = true;
    endReached_ = false;
}

template <class T>
void StringReadStream<T>::Close() {
    input_.clear();
    input_.str("");
    position_ = 0;
    opened_ = false;
    endReached_ = false;
}

template <class T>
bool StringReadStream<T>::IsEndOfStream() const {
    return endReached_;
}

template <class T>
T StringReadStream<T>::Read() {
    EnsureOpened();
    std::string token;
    if (!(input_ >> token)) {
        endReached_ = true;
        throw EndOfStream("StringReadStream reached end");
    }
    ++position_;
    return deserializer_(token);
}

template <class T>
std::size_t StringReadStream<T>::GetPosition() const {
    return position_;
}

template <class T>
bool StringReadStream<T>::IsCanSeek() const {
    return true;
}

template <class T>
std::size_t StringReadStream<T>::Seek(std::size_t index) {
    EnsureOpened();
    input_.clear();
    input_.str(source_);
    position_ = 0;
    endReached_ = false;

    std::string ignored;
    while (position_ < index) {
        if (!(input_ >> ignored)) {
            endReached_ = true;
            throw EndOfStream("Seek index is out of string range");
        }
        ++position_;
    }
    return position_;
}

template <class T>
bool StringReadStream<T>::IsCanGoBack() const {
    return true;
}

template <class T>
void StringReadStream<T>::EnsureOpened() const {
    if (!opened_)
        throw StreamException("Stream is not opened");
}


template <class T>
FileReadStream<T>::FileReadStream(std::string fileName, std::function<T(const std::string&)> itemDeserializer)
    : filename_(std::move(fileName)), deserializer_(std::move(itemDeserializer)), input_(), position_(0),
      opened_(false), endReached_(false) {
    if (!deserializer_)
        throw std::invalid_argument("Deserializer is empty");
}

template <class T>
void FileReadStream<T>::Open() {
    Close();
    input_.open(filename_);
    if (!input_.is_open())
        throw StreamException("Cannot open file for reading: " + filename_);
    position_ = 0;
    opened_ = true;
    endReached_ = false;
}

template <class T>
void FileReadStream<T>::Close() {
    if (input_.is_open())
        input_.close();
    position_ = 0;
    opened_ = false;
    endReached_ = false;
}

template <class T>
bool FileReadStream<T>::IsEndOfStream() const {
    return endReached_;
}

template <class T>
T FileReadStream<T>::Read() {
    EnsureOpened();
    std::string line;
    if (!std::getline(input_, line)) {
        endReached_ = true;
        throw EndOfStream("FileReadStream reached end");
    }
    ++position_;
    return deserializer_(line);
}

template <class T>
std::size_t FileReadStream<T>::GetPosition() const {
    return position_;
}

template <class T>
bool FileReadStream<T>::IsCanSeek() const {
    return true;
}

template <class T>
std::size_t FileReadStream<T>::Seek(std::size_t index) {
    EnsureOpened();
    input_.clear();
    input_.seekg(0, std::ios::beg);
    if (!input_)
        throw StreamException("Cannot seek file: " + filename_);

    position_ = 0;
    endReached_ = false;
    std::string ignored;
    while (position_ < index) {
        if (!std::getline(input_, ignored)) {
            endReached_ = true;
            throw EndOfStream("Seek index is out of file range");
        }
        ++position_;
    }
    return position_;
}

template <class T>
bool FileReadStream<T>::IsCanGoBack() const {
    return true;
}

template <class T>
void FileReadStream<T>::EnsureOpened() const {
    if (!opened_)
        throw StreamException("Stream is not opened");
}


template <class T>
SequenceWriteStream<T>::SequenceWriteStream(Sequence<T>& destinationSequence)
    : destination_(std::shared_ptr<Sequence<T>>(&destinationSequence, [](auto) {})),
      position_(0), opened_(false) {
}

template <class T>
SequenceWriteStream<T>::SequenceWriteStream(std::shared_ptr<Sequence<T>> destinationSequence)
    : destination_(std::move(destinationSequence)), position_(0), opened_(false) {
    if (!destination_)
        throw std::invalid_argument("SequenceWriteStream destination is null");
}

template <class T>
void SequenceWriteStream<T>::Open() {
    position_ = 0;
    opened_ = true;
}

template <class T>
void SequenceWriteStream<T>::Close() {
    position_ = 0;
    opened_ = false;
}

template <class T>
std::size_t SequenceWriteStream<T>::Write(const T& item) {
    EnsureOpened();
    Sequence<T>* result = destination_->Append(item);
    if (result == nullptr)
        throw StreamException("SequenceWriteStream append returned null");
    if (result != destination_.get())
        destination_.reset(result);
    ++position_;
    return position_;
}

template <class T>
std::size_t SequenceWriteStream<T>::GetPosition() const {
    return position_;
}

template <class T>
void SequenceWriteStream<T>::EnsureOpened() const {
    if (!opened_)
        throw StreamException("Stream is not opened");
}


template <class T>
FileWriteStream<T>::FileWriteStream(std::string fileName, std::function<std::string(const T&)> itemSerializer,
                                    bool useAppendMode)
    : filename_(std::move(fileName)), serializer_(std::move(itemSerializer)), output_(), position_(0),
      opened_(false), appendMode_(useAppendMode) {
    if (!serializer_)
        throw std::invalid_argument("Serializer is empty");
}

template <class T>
void FileWriteStream<T>::Open() {
    Close();
    std::ios::openmode mode = std::ios::out;
    if (appendMode_)
        mode |= std::ios::app;
    output_.open(filename_, mode);
    if (!output_.is_open())
        throw StreamException("Cannot open file for writing: " + filename_);
    position_ = 0;
    opened_ = true;
}

template <class T>
void FileWriteStream<T>::Close() {
    if (output_.is_open())
        output_.close();
    position_ = 0;
    opened_ = false;
}

template <class T>
std::size_t FileWriteStream<T>::Write(const T& item) {
    EnsureOpened();
    output_ << serializer_(item) << '\n';
    if (!output_)
        throw StreamException("Cannot write to file: " + filename_);
    ++position_;
    return position_;
}

template <class T>
std::size_t FileWriteStream<T>::GetPosition() const {
    return position_;
}

template <class T>
void FileWriteStream<T>::EnsureOpened() const {
    if (!opened_)
        throw StreamException("Stream is not opened");
}
