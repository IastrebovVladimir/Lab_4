#ifndef DYNAMICARRAY_H
#define DYNAMICARRAY_H

#include <stdexcept>

template <typename T>
class DynamicArray {
private:
    T* data;
    int size;
    int capacity;
public:
    DynamicArray() : size(0), capacity(0), data(nullptr) {}
    DynamicArray(int size) : size(size), capacity(size), data(new T[size]()) {}

    DynamicArray(const DynamicArray<T>& dynamicArray)
        : size(dynamicArray.size), capacity(dynamicArray.capacity), data(new T[dynamicArray.size]) {
        for (int index = 0; index < dynamicArray.size; index++)
            data[index] = dynamicArray.data[index];
    };

    const T &Get(int index) const {
        if (index < 0 || index >= size) {
            throw std::out_of_range("Index out of range in DynamicArray::Get");
        }
        return data[index];
    }

    void Set(int index, const T &value) const {
        if (index < 0 || index >= size) {
            throw std::out_of_range("Index out of range in DynamicArray::Set");
        }
        data[index] = value;
    }

    int GetSize() const {
        return size;
    }

    int GetCapacity() const {
        return capacity;
    }

    DynamicArray& operator=(const DynamicArray& other) {
        if (this == &other) {
            return *this;
        }
        T* newData = new T[other.capacity]();
        for (int i = 0; i < other.size; ++i) {
            newData[i] = other.data[i];
        }
        delete[] data;
        data = newData;
        size = other.size;
        capacity = other.capacity;
        return *this;
    }

    void ReSize(int newSize) {
        if (newSize < capacity) {
            size = newSize;
            return;
        }

        int newCapacity = (capacity == 0 ? 1 : capacity);
        while (newCapacity < newSize)
            newCapacity *= 2;

        T* newData = new T[newCapacity]();
        for (int index = 0; index < size; index++)
            newData[index] = data[index];
        delete[] data;
        data = newData;
        size = newSize;
        capacity = newCapacity;
    }

    ~DynamicArray() {
        delete[] data;
    }
};

#endif