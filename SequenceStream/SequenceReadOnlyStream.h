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
    Enumerator<T>* enumerator;
    size_t position;
    bool isOpen;
    bool atEnd;

public:
    explicit SequenceReadOnlyStream(Sequence<T>* sequence)
            : sequence(sequence), enumerator(nullptr), position(0), isOpen(false), atEnd(false) {}

    bool IsEndOfStream() const override {
        return atEnd;
    }

    T Read() override {
        if (!isOpen) {
            throw std::logic_error("SequenceReadOnlyStream: stream is not open");
        }
        if (atEnd) {
            throw std::runtime_error("End of stream");
        }
        if (sequence == nullptr) {
            throw std::logic_error("SequenceReadOnlyStream: sequence is null");
        }
        if (enumerator == nullptr) {
            throw std::logic_error("SequenceReadOnlyStream: enumerator is not initialized");
        }

        if (!enumerator->MoveNext()) {
            atEnd = true;
            throw std::runtime_error("End of stream");
        }

        position++;
        return enumerator->Current();
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
        if (sequence == nullptr) {
            throw std::logic_error("SequenceReadOnlyStream: sequence is null");
        }

        delete enumerator;
        enumerator = new Enumerator<T>(sequence->GetEnumerator());

        position = 0;
        isOpen = true;
        atEnd = false;
    }

    void Close() override {
        isOpen = false;
        atEnd = false;
        position = 0;

        delete enumerator;
        enumerator = nullptr;
    }
};

#endif
