#ifndef LISTSEQUENCE_H
#define LISTSEQUENCE_H

#include "LinkedList.h"
#include "Sequence.h"

template <typename T>
class ListSequence : public Sequence<T> {
protected:
    LinkedList<T>* items;

    ListSequence(const ListSequence<T>& listSequence)
            : items(new LinkedList<T>(*listSequence.items)) {}

    ListSequence(T* data, int count)
            : items(new LinkedList<T>()) {
        for (int index = 0; index < count; index++)
            items->Append(data[index]);
    }

    ListSequence(const LinkedList<T>& linkedList)
            : items(new LinkedList<T>(linkedList)) {}

public:
    ListSequence() : items(new LinkedList<T>()) {}

    Enumerator<T> GetEnumerator() const override {
        return items->GetEnumerator();
    }

    const T& GetFirst() const override {
        return items->GetFirst();
    }

    const T& GetLast() const override {
        return items->GetLast();
    }

    int GetLength() const override {
        return items->GetLength();
    }

    ~ListSequence() override {
        delete items;
    }
};

template <typename T>
class MutableListSequence : public ListSequence<T> {
protected:
    Sequence<T>* CreateEmpty() const override {
        return new MutableListSequence<T>();
    }
public:
    template<typename T2> Sequence<T2>* Map(T2 (*func)(T)) const {
        auto* result = new MutableListSequence<T2>();
        auto it = this->GetEnumerator();
        while (it.MoveNext()) {
            result->Append(func(it.Current()));
        }
        return result;
    }
    MutableListSequence() : ListSequence<T>() {}
    MutableListSequence(T* data, int count) : ListSequence<T>(data, count) {}
    MutableListSequence(const ListSequence<T>& other) : ListSequence<T>(other) {}
    MutableListSequence(const LinkedList<T>& linkedList) : ListSequence<T>(linkedList) {}

    Sequence<T>* Append(const T& item) override {
        this->items->Append(item);
        return this;
    }

    Sequence<T>* Prepend(const T &item) override {
        this->items->Prepend(item);
        return this;
    }

    Sequence<T>* InsertAt(const T &item, int index) override {
        this->items->InsertAt(item, index);
        return this;
    }
};

template <typename T>
class ImmutableListSequence : public ListSequence<T> {
protected:
    Sequence<T>* CreateEmpty() const override {
        return new ImmutableListSequence<T>();
    }
public:
    template<typename T2> Sequence<T2>* Map(T2 (*func)(T)) const {
        auto* result = new ImmutableListSequence<T2>();
        auto it = this->GetEnumerator();
        while (it.MoveNext()) {
            result->Append(func(it.Current()));
        }
        return result;
    }
    ImmutableListSequence() : ListSequence<T>() {}
    ImmutableListSequence(T* data, int count) : ListSequence<T>(data, count) {}
    ImmutableListSequence(const ListSequence<T>& other) : ListSequence<T>(other) {}
    ImmutableListSequence(const LinkedList<T>& linkedList) : ListSequence<T>(linkedList) {}

    Sequence<T>* Append(const T &item) override {
        auto* newList = new ImmutableListSequence<T>(*this);
        newList->items->Append(item);
        return newList;
    }

    Sequence<T>* Prepend(const T &item) override {
        auto* newList = new ImmutableListSequence<T>(*this);
        newList->items->Prepend(item);
        return newList;
    }

    Sequence<T>* InsertAt(const T &item, int index) override {
        auto* newList = new ImmutableListSequence<T>(*this);
        newList->items->InsertAt(item, index);
        return newList;
    }
};

#endif
