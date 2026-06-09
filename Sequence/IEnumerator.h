#ifndef ENUMERATOR_H
#define ENUMERATOR_H

template<typename T>
class IEnumerator {
public:
    virtual bool MoveNext() = 0;
    virtual const T& Current() const = 0;
    virtual ~IEnumerator() = default;
};

template<typename T>
class Enumerator {
private:
    IEnumerator<T>* ptr;

public:
    explicit Enumerator(IEnumerator<T>* ptr) : ptr(ptr) {}

    ~Enumerator() {
        delete ptr;
    }

    bool MoveNext() {
        return ptr->MoveNext();
    }

    const T& Current() const {
        return ptr->Current();
    }

    Enumerator(const Enumerator&) = delete;
    Enumerator& operator=(const Enumerator&) = delete;

    Enumerator(Enumerator&& other): ptr(other.ptr) {
        other.ptr = nullptr;
    }

    Enumerator& operator=(Enumerator&& other) {
        if (this != &other) {
            delete ptr;
            ptr = other.ptr;
            other.ptr = nullptr;
        }
        return *this;
    }
};

#endif