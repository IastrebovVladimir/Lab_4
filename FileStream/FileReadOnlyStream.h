#ifndef FILE_READ_ONLY_STREAM_H
#define FILE_READ_ONLY_STREAM_H

#include <cstddef>
#include <fstream>
#include <stdexcept>
#include <string>
#include "../Stream/ReadOnlyStream.h"
#include "../Sequence/Sequence.h"
#include "../Sequence/IEnumerator.h"

template <typename T>
class FileReadOnlyStream : public ReadOnlyStream<T> {
private:
    std::string filename;
    T (*fileParser)(const std::string&);
    std::ifstream file;
    size_t position;
    bool isOpen;
    bool atEnd;

public:
    FileReadOnlyStream(const std::string& filename, T (*fileParser)(const std::string&))
            : filename(filename), fileParser(fileParser), position(0), isOpen(false), atEnd(false) {}

    bool IsEndOfStream() const override {
        return atEnd;
    }

    T Read() override {
        if (!isOpen) {
            throw std::logic_error("FileReadOnlyStream: Stream is not open");
        }
        if (atEnd) {
            throw std::runtime_error("FileReadOnlyStream: End of stream");
        }

        std::string line;
        if (!std::getline(file, line)) {
            atEnd = true;
            throw std::runtime_error("FileReadOnlyStream: End of stream");
        }

        position++;
        return fileParser(line);
    }

    size_t GetPosition() const override {
        return position;
    }

    bool IsCanSeek() const override {
        return false;
    }

    size_t Seek(size_t) override {
        throw std::logic_error("FileReadOnlyStream: seek is not supported");
    }

    bool IsCanGoBack() const override {
        return false;
    }

    void Open() override {
        file.open(filename);

        if (!file.is_open()) {
            throw std::runtime_error("FileReadOnlyStream: cannot open file");
        }

        position = 0;
        isOpen = true;
        atEnd = false;
    }

    void Close() override {
        if (file.is_open()) {
            file.close();
        }
        isOpen = false;
    }
};

#endif
