#ifndef ARRAYSEQUENCE_H
#define ARRAYSEQUENCE_H

#include "DynamicArray.h"
#include "Sequence.h"


template <typename T>
class ArraySequence: public Sequence<T> {
protected:
    DynamicArray<T>* items;

    ArraySequence() : items(new DynamicArray<T>(0)) {}

    ArraySequence(T* data, int count) : items(new DynamicArray<T>(count)) {
        for (int index = 0; index < count; index++)
            items->Set(index, data[index]);
    }

    ArraySequence(const ArraySequence<T>& arraySequence)
            : items(new DynamicArray<T>(*arraySequence.items)) {}

    ArraySequence(const DynamicArray<T>& dynamicArray)
            : items(new DynamicArray<T>(dynamicArray)) {}

    void copyRange(DynamicArray<T>* dynamicArray,
                   int destinationIndex,
                   int sourceIndex,
                   int count) {
        for (int index = 0; index < count; index++)
            dynamicArray->Set(destinationIndex + index, Get(sourceIndex + index));
    }

    void appendItems(ArraySequence<T>* destination,
                   const ArraySequence<T>* source,
                   int sourceIndex,
                   int count) const {
        for (int index = 0; index < count; index++) {
            destination->Append(source->Get(sourceIndex + index));
        }
    }

    class ArrayEnumerator : public IEnumerator<T> {
    private:
        const ArraySequence<T>* sequence;
        int index;

    public:
        explicit ArrayEnumerator(const ArraySequence<T>* arraySequence)
                : sequence(arraySequence), index(-1) {}

        bool MoveNext() override {
            if (index + 1 < sequence->GetLength()) {
                ++index;
                return true;
            }
            return false;
        }

        const T& Current() const override {
            if (index < 0 || index >= sequence->GetLength()) {
                throw std::out_of_range("Current::invalid position");
            }
            return sequence->Get(index);
        }
    };

public:
    Enumerator<T> GetEnumerator() const override {
        return Enumerator<T>(new ArrayEnumerator(this));
    }
    ArraySequence& operator=(const ArraySequence& other) {
        if (this == &other)
            return *this;
        auto* newItems = new DynamicArray<T>(*other.items);
        delete items;
        items = newItems;
        return *this;
    }

    const T& GetFirst() const override {
        if (GetLength() == 0)
            throw std::out_of_range("ArraySequence::GetFirst: sequence is empty");
        return items->Get(0);
    }

    const T& GetLast() const override {
        if (GetLength() == 0)
            throw std::out_of_range("ArraySequence::GetLast: sequence is empty");
        return items->Get(GetLength() - 1);
    }

    const T& Get(int index) const {
        if (index < 0 || index >= GetLength())
            throw std::out_of_range("ArraySequence::Get: index out of range");
        return items->Get(index);
    }

    Sequence<T>* GetSubsequence(int startIndex, int endIndex) const override {
        if (startIndex < 0 || endIndex < 0) {
            throw std::out_of_range("GetSubsequence: index out of range");
        }
        if (startIndex > endIndex) {
            throw std::out_of_range("GetSubsequence: startIndex > endIndex");
        }
        if (endIndex >= GetLength()) {
            throw std::out_of_range("GetSubsequence: endIndex out of range");
        }
        Sequence<T>* result = this->CreateEmpty();
        for (int index = startIndex; index <= endIndex; index++) {
            result->Append(this->Get(index));
        }
        return result;
    }

    int GetLength() const override {
        return items->GetSize();
    }

    ~ArraySequence() override {
        delete items;
    }
};

template <typename T>
class MutableArraySequence: public ArraySequence<T> {
protected:
    Sequence<T>* CreateEmpty() const override {
        return new MutableArraySequence<T>();
    }
public:
    template<typename T2> Sequence<T2>* Map(T2 (*func)(T)) const {
        auto* result = new MutableArraySequence<T2>();
        auto it = this->GetEnumerator();
        while (it.MoveNext()) {
            result->Append(func(it.Current()));
        }
        return result;
    }
    MutableArraySequence() : ArraySequence<T>() {}

    MutableArraySequence(T* data, int count) : ArraySequence<T>(data, count) {}

    MutableArraySequence(const ArraySequence<T>& arraySequence)
            : ArraySequence<T>(arraySequence) {}

    Sequence<T>* Append(const T &item) override {
        this->items->ReSize(this->GetLength() + 1);
        this->items->Set(this->GetLength() - 1, item);
        return this;
    }

    Sequence<T>* Prepend(const T &item) override {
        int newSize = this->GetLength() + 1;
        auto* newItems = new DynamicArray<T>(newSize);
        newItems->Set(0, item);
        this->copyRange(newItems, 1, 0, this->GetLength());
        delete this->items;
        this->items = newItems;
        return this;
    }

    Sequence<T>* InsertAt(const T &item, int index) override {
        if (index < 0 || index > this->GetLength())
            throw std::out_of_range("ArraySequence::Get: index out of range");

        int newSize = this->GetLength() + 1;
        auto* newItems = new DynamicArray<T>(newSize);

        this->copyRange(newItems, 0, 0, index);
        newItems->Set(index, item);
        this->copyRange(newItems, index + 1, index, this->GetLength() - index);

        delete this->items;
        this->items = newItems;
        return this;
    }

    MutableArraySequence(const DynamicArray<T>& dynamicArray)
            : ArraySequence<T>(dynamicArray) {}
};

template <typename T>
class ImmutableArraySequence: public ArraySequence<T> {
protected:
    Sequence<T>* CreateEmpty() const override {
        return new ImmutableArraySequence<T>();
    }
public:
    template<typename T2> Sequence<T2>* Map(T2 (*func)(T)) const {
        auto* result = new ImmutableArraySequence<T2>();
        auto it = this->GetEnumerator();
        while (it.MoveNext()) {
            result->Append(func(it.Current()));
        }
        return result;
    }

    ImmutableArraySequence() : ArraySequence<T>() {}

    ImmutableArraySequence(T* data, int count)
            : ArraySequence<T>(data, count) {}

    ImmutableArraySequence(const ArraySequence<T>& arraySequence)
            : ArraySequence<T>(arraySequence) {}

    Sequence<T>* Append(const T &item) override {
        auto* newArray = new ImmutableArraySequence<T>(*this);
        newArray->items->ReSize(newArray->GetLength() + 1);
        newArray->items->Set(newArray->GetLength() - 1, item);
        return newArray;
    }

    Sequence<T>* Prepend(const T &item) override {
        int newSize = this->GetLength() + 1;
        auto* newItems = new DynamicArray<T>(newSize);
        newItems->Set(0, item);
        this->copyRange(newItems, 1, 0, this->GetLength());
        auto* newArray = new ImmutableArraySequence<T>(*newItems);
        delete newItems;
        return newArray;
    }

    Sequence<T>* InsertAt(const T &item, int index) override {
        if (index < 0 || index > this->GetLength())
            throw std::out_of_range("ArraySequence::Get: index out of range");

        int newSize = this->GetLength() + 1;
        auto* newItems = new DynamicArray<T>(newSize);

        this->copyRange(newItems, 0, 0, index);
        newItems->Set(index, item);
        this->copyRange(newItems, index + 1, index, this->GetLength() - index);

        auto* newArray = new ImmutableArraySequence<T>(*newItems);
        delete newItems;
        return newArray;
    }

    ImmutableArraySequence(const DynamicArray<T>& dynamicArray)
            : ArraySequence<T>(dynamicArray) {}
};

#endif
