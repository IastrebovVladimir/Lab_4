#ifndef FILE_WRITE_ONLY_STREAM_H
#define FILE_WRITE_ONLY_STREAM_H

#include <cstddef>
#include <fstream>
#include <stdexcept>
#include <string>
#include "../Stream/WriteOnlyStream.h"

template <typename T>
class FileWriteOnlyStream : public WriteOnlyStream<T> {
private:
    std::string filename;
    std::string (*serializer)(const T&);
    std::ofstream file;
    size_t position;
    bool isOpen;

public:
    FileWriteOnlyStream(const std::string& filename, std::string (*serializer)(const T&))
            : filename(filename), serializer(serializer), position(0), isOpen(false) {}

    size_t GetPosition() const override {
        return position;
    }

    size_t Write(const T& value) override {
        if (!isOpen) {
            throw std::logic_error("FileWriteOnlyStream: stream is not open");
        }

        file << serializer(value) << '\n';

        if (!file) {
            throw std::runtime_error("FileWriteOnlyStream: write failed");
        }

        position++;
        return position;
    }

    void Open() override {
        file.open(filename);

        if (!file.is_open()) {
            throw std::runtime_error("FileWriteOnlyStream: cannot open file");
        }

        isOpen = true;
        position = 0;
    }

    void Close() override {
        if (file.is_open()) {
            file.close();
        }
        isOpen = false;
    }
};

#endif