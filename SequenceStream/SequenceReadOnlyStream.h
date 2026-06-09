#ifndef SEQUENCE_READ_ONLY_STREAM_H
#define SEQUENCE_READ_ONLY_STREAM_H

#include <cstddef>
#include <stdexcept>

#include "../Stream//ReadOnlyStream.h"
#include "../Sequence/Sequence.h"
#include "../Sequence/ArraySequence.h"

template <typename T>
class SequenceReadOnlyStream : public ReadOnlyStream<T> {
private:
    Sequence<T>* sequence;
    size_t position;
    bool isOpen;
    bool atEnd;

public:
    explicit SequenceReadOnlyStream(Sequence<T>* sequence)
            : sequence(sequence), position(0), isOpen(false), atEnd(false) {}

    bool IsEndOfStream() const override {
        return atEnd;
    }

    T Read() override {
        if (!isOpen) {
            throw std::logic_error("Stream is not open");
        }
        if (atEnd) {
            throw std::runtime_error("End of stream");
        }

        auto* arr = dynamic_cast<ArraySequence<T>*>(sequence);
        if (!arr) {
            throw std::logic_error("SequenceReadOnlyStream expects ArraySequence");
        }

        if (position >= static_cast<size_t>(arr->GetLength())) {
            atEnd = true;
            throw std::runtime_error("End of stream");
        }

        T value = arr->Get(static_cast<int>(position++));
        // после успешного чтения atEnd остаётся false
        return value;
    }

    size_t GetPosition() const override {
        return position;
    }

    bool IsCanSeek() const override {
        return false;
    }

    size_t Seek(size_t) override {
        throw std::logic_error("SequenceReadOnlyStream: seek is not supported");
    }

    bool IsCanGoBack() const override {
        return false;
    }

    void Open() override {
        if (!sequence) {
            throw std::logic_error("Sequence is null");
        }
        isOpen = true;
        position = 0;
        atEnd = false;
    }

    void Close() override {
        isOpen = false;
    }
};

#endif
