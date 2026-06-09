#ifndef SEQUENCE_WRITE_ONLY_STREAM_H
#define SEQUENCE_WRITE_ONLY_STREAM_H

#include <cstddef>
#include <stdexcept>
#include "../Stream/WriteOnlyStream.h"
#include "../Sequence/ArraySequence.h"

template <typename T>
class SequenceWriteOnlyStream : public WriteOnlyStream<T> {
private:
    MutableArraySequence<T>* sequence;
    size_t position;
    bool isOpen;

public:
    explicit SequenceWriteOnlyStream(MutableArraySequence<T>* sequence)
            : sequence(sequence), position(static_cast<size_t>(sequence->GetLength())), isOpen(false) {}

    ~SequenceWriteOnlyStream() override = default;

    size_t GetPosition() const override {
        return position;
    }

    size_t Write(const T& value) override {
        if (!isOpen) {
            throw std::logic_error("SequenceWriteOnlyStream: stream is not open");
        }

        sequence->Append(value);
        position = static_cast<size_t>(sequence->GetLength());
        return position;
    }

    void Open() override {
        isOpen = true;
    }

    void Close() override {
        isOpen = false;
    }

    MutableArraySequence<T>* GetSequence() const {
        return sequence;
    }
};

#endif