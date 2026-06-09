#ifndef WRITEONLYSTREAM_H
#define WRITEONLYSTREAM_H

#include <cstddef>

template <typename T>
class WriteOnlyStream {
public:
    virtual ~WriteOnlyStream() = default;

    virtual size_t GetPosition() const = 0;
    virtual size_t Write(const T& item) = 0;

    virtual void Open() = 0;
    virtual void Close() = 0;
};

#endif
