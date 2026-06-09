#ifndef SEQUENCE_H
#define SEQUENCE_H

#include <stdexcept>
#include "Option.h"
#include "IEnumerator.h"

template <class T>
class Sequence {
protected:
    virtual Sequence<T>* CreateEmpty() const = 0;
public:
    virtual Enumerator<T> GetEnumerator() const = 0;
    virtual const T &GetFirst() const = 0;
    virtual const T &GetLast() const = 0;
    virtual Sequence<T>* GetSubsequence(int startIndex, int endIndex) const {
        if (startIndex < 0 || endIndex < 0) {
            throw std::out_of_range("GetSubsequence: index out of range");
        }
        if (startIndex > endIndex) {
            throw std::out_of_range("GetSubsequence: startIndex > endIndex");
        }
        if (endIndex >= GetLength()) {
            throw std::out_of_range("GetSubsequence: endIndex out of range");
        }
        Sequence<T>* result = CreateEmpty();

        auto iterator = this->GetEnumerator();
        int index = 0;

        while (iterator.MoveNext()) {
            if (index >= startIndex && index <= endIndex) {
                result->Append(iterator.Current());
            }
            if (index > endIndex) {
                break;
            }
            index++;
        }

        return result;
    }
    virtual int GetLength() const = 0;

    virtual Sequence<T>* Append(const T &item) = 0;
    virtual Sequence<T>* Prepend(const T &item) = 0;
    virtual Sequence<T>* InsertAt(const T &item, int index) = 0;

    Sequence<T>* Concat(const Sequence<T>* other) const {
        Sequence<T>* result = CreateEmpty();
        auto iterator = this->GetEnumerator();
        while (iterator.MoveNext()) {
            result->Append(iterator.Current());
        }

        iterator = other->GetEnumerator();
        while (iterator.MoveNext()) {
            result->Append(iterator.Current());
        }

        return result;
    }

    Sequence<T>* Slice(int startIndex, int count, const Sequence<T>& replacement) const {
        if (startIndex < 0 || startIndex > GetLength()) {
            throw std::out_of_range("Slice: startIndex out of range");
        }
        if (count < 0) {
            throw std::out_of_range("Slice: count cannot be negative");
        }
        if (startIndex + count > GetLength()) {
            throw std::out_of_range("Slice: range out of bounds");
        }

        Sequence<T>* result = CreateEmpty();
        auto iterator = this->GetEnumerator();
        for (int index = 0; index < startIndex && iterator.MoveNext(); index++) {
            result->Append(iterator.Current());
        }

        auto replacementIterator = replacement.GetEnumerator();
        while (replacementIterator.MoveNext()) {
            result->Append(replacementIterator.Current());
        }

        for (int index = 0; index < count && iterator.MoveNext(); index++) {
        }

        while (iterator.MoveNext()) {
            result->Append(iterator.Current());
        }

        return result;
    }

    template <typename T2> T2 Reduce(T2 (*func)(const T2&, const T&), const T2& initial) const {
        T2 result = initial;
        Enumerator<T> iterator = GetEnumerator();

        while (iterator.MoveNext()) {
            result = func(result, iterator.Current());
        }
        return result;
    }

    Sequence<T>* Where(bool (*predicate)(const T&)) const {
        Sequence<T>* result = CreateEmpty();
        auto iterator = this->GetEnumerator();
        while (iterator.MoveNext()) {
            if (predicate(iterator.Current())) {
                result->Append(iterator.Current());
            }
        }
        return result;
    }

    Option<T> GetFirstPred(bool (*predicate)(const T&) = nullptr) const {
        auto iterator = GetEnumerator();
        if (predicate == nullptr) {
            while (iterator.MoveNext())
                return Option<T>(iterator.Current());
            return Option<T>();
        }

        while (iterator.MoveNext()) {
            if (predicate(iterator.Current())) {
                return Option<T>(iterator.Current());
            }
        }
        return Option<T>();

    }

    Option<T> GetLastPred(bool (*predicate)(const T&) = nullptr) const {
        Option<T> last;
        auto iterator = GetEnumerator();

        if (predicate == nullptr) {
            while (iterator.MoveNext()) {
                last = Option<T>(iterator.Current());
            }
            return last;
        }
        while (iterator.MoveNext()) {
            if (predicate(iterator.Current())) {
                last = Option<T>(iterator.Current());
            }
        }

        return last;
    }

    virtual ~Sequence() = default;
};


#endif
