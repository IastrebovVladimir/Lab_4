#ifndef LAZYSEQUENCE_H
#define LAZYSEQUENCE_H

#include <memory>
#include <limits>
#include <stdexcept>
#include <utility>

#include "../Sequence/Sequence.h"
#include "../Sequence/ArraySequence.h"
#include "Generator.h"
#include "../TransfiniteNumbers/Cardinal.h"
#include "../TransfiniteNumbers/Ordinal.h"

template <class T>
class LazySequence : public Sequence<T> {
private:
    mutable MutableArraySequence<T> materialized;
    mutable std::shared_ptr<Generator<T>> generator;
    bool isInfinite = false;
    Ordinal orderType = Ordinal::MakeFinite(0);

    std::shared_ptr<LazySequence<T>> left;
    std::shared_ptr<LazySequence<T>> right;

    static MutableArraySequence<T> CopyToMutable(const Sequence<T>& source) {
        MutableArraySequence<T> temp;
        auto it = source.GetEnumerator();
        while (it.MoveNext()) {
            temp.Append(it.Current());
        }
        return temp;
    }

    bool IsConcat() const {
        return left != nullptr && right != nullptr;
    }

    static int ToIntIndex(size_t value, const char* msg) {
        if (value > static_cast<size_t>(std::numeric_limits<int>::max())) {
            throw std::out_of_range(msg);
        }
        return static_cast<int>(value);
    }

    void MaterializeTo(int index) const {
        if (index < 0) {
            throw std::out_of_range("LazySequence: negative index");
        }

        if (IsConcat()) {
            throw std::logic_error("LazySequence: plain operation is invalid for concat");
        }

        while (materialized.GetLength() <= index) {
            if (!generator || !generator->HasNext(materialized)) {
                throw std::out_of_range("LazySequence: index out of range");
            }
            materialized.Append(generator->GetNext(materialized));
        }
    }

    void MaterializeAll() const {
        if (IsConcat()) {
            throw std::logic_error("LazySequence: plain operation is invalid for concat");
        }

        while (generator && generator->HasNext(materialized)) {
            materialized.Append(generator->GetNext(materialized));
        }
    }

    static std::shared_ptr<LazySequence<T>> MakeFiniteSingle(const T& item) {
        auto sequence = std::make_shared<LazySequence<T>>();
        sequence->materialized.Append(item);
        sequence->generator = std::make_shared<EmptyGenerator<T>>();
        sequence->isInfinite = false;
        sequence->orderType = Ordinal::MakeFinite(1);
        return sequence;
    }

    static std::shared_ptr<LazySequence<T>> MakeFiniteFromSequence(const Sequence<T>& sequence) {
        return std::make_shared<LazySequence<T>>(sequence);
    }

    class LazyEnumerator : public IEnumerator<T> {
    private:
        const LazySequence<T>* sequence;
        int index;

    public:
        explicit LazyEnumerator(const LazySequence<T>* seq)
                : sequence(seq), index(-1) {}

        bool MoveNext() override {
            if (index == -2) {
                return false;
            }

            try {
                sequence->Get(index + 1);
                index++;
                return true;
            } catch (const std::out_of_range&) {
                index = -2;
                return false;
            }
        }

        const T& Current() const override {
            if (index < 0) {
                throw std::out_of_range("LazyEnumerator: no current element");
            }
            return sequence->Get(index);
        }
    };

protected:
    Sequence<T>* CreateEmpty() const override {
        return new LazySequence<T>();
    }

public:
    LazySequence() : generator(std::make_shared<EmptyGenerator<T>>()), isInfinite(false), orderType(Ordinal::MakeFinite(0)) {}

    LazySequence(T* items, int count) : materialized(items, count), generator(std::make_shared<EmptyGenerator<T>>()), isInfinite(false), orderType(Ordinal::MakeFinite(static_cast<size_t>(count))) {}

    LazySequence(const Sequence<T>& sequence) {
        MutableArraySequence<T> temp;
        auto it = sequence.GetEnumerator();
        while (it.MoveNext()) {
            temp.Append(it.Current());
        }

        materialized = temp;
        generator = std::make_shared<EmptyGenerator<T>>();
        isInfinite = false;
        orderType = Ordinal::MakeFinite(static_cast<size_t>(temp.GetLength()));
    }

    LazySequence(const MutableArraySequence<T>& prefix, std::shared_ptr<Generator<T>> gen, bool infinite = false) : materialized(prefix), generator(gen ? std::move(gen) : std::make_shared<EmptyGenerator<T>>()), isInfinite(infinite) {
        if (isInfinite) {
            orderType = Ordinal::MakeOmega();
        } else {
            MaterializeAll();
            orderType = Ordinal::MakeFinite(static_cast<size_t>(materialized.GetLength()));
        }
    }

    template <typename Rule>
    LazySequence(Rule ruleFunction, const Sequence<T>& prefix, bool infinite = false) : materialized(CopyToMutable(prefix)), generator(std::make_shared<RuleGenerator<T, Rule>>(ruleFunction)), isInfinite(infinite) {
        if (isInfinite) {
            orderType = Ordinal::MakeOmega();
        } else {
            MaterializeAll();
            orderType = Ordinal::MakeFinite(static_cast<size_t>(materialized.GetLength()));
        }
    }

    LazySequence(std::shared_ptr<LazySequence<T>> leftSeq, std::shared_ptr<LazySequence<T>> rightSeq) : generator(nullptr), isInfinite(false), left(std::move(leftSeq)), right(std::move(rightSeq)) {
        if (!left || !right)
            throw std::logic_error("LazySequence: concat requires both left and right");
        orderType = Ordinal::Add(left->GetOrderType(), right->GetOrderType());
    }

    int GetLength() const override {
        if (IsConcat()) {
            Ordinal ord = GetOrderType();
            if (!Ordinal::IsFinite(ord)) {
                throw std::logic_error("LazySequence: sequence has no finite length");
            }

            size_t len = Ordinal::GetFinitePart(ord);
            return ToIntIndex(len, "LazySequence: length is too large");
        }

        if (isInfinite) {
            throw std::logic_error("LazySequence: infinite sequence has no finite length");
        }

        MaterializeAll();
        return materialized.GetLength();
    }

    const T& Get(int index) const {
        if (index < 0) {
            throw std::out_of_range("LazySequence: negative index");
        }

        if (IsConcat()) {
            return GetOrdinal(Ordinal::MakeFinite(static_cast<size_t>(index)));
        }

        MaterializeTo(index);
        return materialized.Get(index);
    }

    const T& GetFirst() const override {
        return Get(0);
    }

    const T& GetLast() const override {
        if (IsConcat()) {
            Ordinal ord = GetOrderType();
            if (!Ordinal::IsFinite(ord)) {
                throw std::logic_error("LazySequence: transfinite sequence has no finite last element");
            }

            size_t len = Ordinal::GetFinitePart(ord);
            if (len == 0) {
                throw std::out_of_range("LazySequence: empty sequence");
            }

            return Get(ToIntIndex(len - 1, "LazySequence: length is too large"));
        }

        if (isInfinite) {
            throw std::logic_error("LazySequence: infinite sequence has no last element");
        }

        MaterializeAll();
        if (materialized.GetLength() == 0) {
            throw std::out_of_range("LazySequence: empty sequence");
        }

        return materialized.GetLast();
    }

    const T& GetOrdinal(const Ordinal& index) const {
        if (!IsConcat()) {
            if (!Ordinal::IsFinite(index)) {
                throw std::out_of_range("LazySequence: base sequence supports only finite ordinals");
            }

            size_t finiteIndex = Ordinal::GetFinitePart(index);
            return Get(ToIntIndex(finiteIndex, "LazySequence: ordinal index is too large"));
        }

        Ordinal leftOrd = left->GetOrderType();

        if (Ordinal::Compare(index, leftOrd) < 0) {
            return left->GetOrdinal(index);
        }

        Ordinal remainder;
        if (Ordinal::TryRemovePrefix(index, leftOrd, remainder)) {
            return right->GetOrdinal(remainder);
        }

        throw std::out_of_range("LazySequence: ordinal index out of range");
    }

    Sequence<T>* Append(const T& item) override {
        if (!IsConcat() && isInfinite) {
            auto selfCopy = std::make_shared<LazySequence<T>>(*this);
            auto tail = MakeFiniteSingle(item);
            return new LazySequence<T>(selfCopy, tail);
        }

        if (IsConcat()) {
            Sequence<T>* appendedBase = right->Append(item);
            auto* appendedRight = dynamic_cast<LazySequence<T>*>(appendedBase);
            if (!appendedRight) {
                delete appendedBase;
                throw std::logic_error("LazySequence::Append: right append failed");
            }
            return new LazySequence<T>(left, std::shared_ptr<LazySequence<T>>(appendedRight));
        }

        auto* result = new LazySequence<T>();
        result->materialized = materialized;
        result->isInfinite = false;
        result->orderType = Ordinal::Add(orderType, Ordinal::MakeFinite(1));

        auto sourceClone = generator ? generator->Clone() : std::make_shared<EmptyGenerator<T>>();
        result->generator = std::make_shared<AppendGenerator<T>>(sourceClone, item);

        return result;
    }

    Sequence<T>* Prepend(const T& item) override {
        if (IsConcat()) {
            auto head = MakeFiniteSingle(item);
            return new LazySequence<T>(head, std::make_shared<LazySequence<T>>(*this));
        }

        auto* result = new LazySequence<T>();
        result->isInfinite = isInfinite;
        result->orderType = Ordinal::IsFinite(orderType)
                            ? Ordinal::Add(Ordinal::MakeFinite(1), orderType)
                            : orderType;

        MutableArraySequence<T> newMat;
        newMat.Append(item);
        for (int i = 0; i < materialized.GetLength(); ++i) {
            newMat.Append(materialized.Get(i));
        }
        result->materialized = newMat;

        auto sourceClone = generator ? generator->Clone() : std::make_shared<EmptyGenerator<T>>();
        result->generator = std::make_shared<PrependGenerator<T>>(sourceClone, item);

        return result;
    }

    Sequence<T>* InsertAt(const T& item, int index) override {
        if (index < 0) {
            throw std::out_of_range("LazySequence: negative index");
        }

        if (IsConcat()) {
            throw std::logic_error("LazySequence: InsertAt for concat is not implemented");
        }

        if (isInfinite) {
            if (index > 0 && materialized.GetLength() < index) {
                MaterializeTo(index - 1);
            }
        } else {
            MaterializeAll();
            if (index > materialized.GetLength()) {
                throw std::out_of_range("LazySequence: index out of range");
            }
        }

        auto* result = new LazySequence<T>();

        MutableArraySequence<T> newMaterialized;
        int prefixLen = materialized.GetLength();

        int leftCount = (index < prefixLen ? index : prefixLen);
        for (int i = 0; i < leftCount; ++i) {
            newMaterialized.Append(materialized.Get(i));
        }

        if (!isInfinite && index > prefixLen) {
            throw std::out_of_range("LazySequence: index out of range");
        }

        newMaterialized.Append(item);

        for (int i = leftCount; i < prefixLen; ++i) {
            newMaterialized.Append(materialized.Get(i));
        }

        result->materialized = newMaterialized;

        auto sourceClone = generator ? generator->Clone() : std::make_shared<EmptyGenerator<T>>();
        result->generator = std::make_shared<InsertGenerator<T>>(sourceClone, item, index);

        if (isInfinite) {
            result->isInfinite = true;
            result->orderType = orderType;
        } else {
            result->isInfinite = false;
            result->orderType = Ordinal::Add(orderType, Ordinal::MakeFinite(1));
        }

        return result;
    }

    Sequence<T>* Concat(const Sequence<T>* other) const {
        auto* otherLazy = dynamic_cast<const LazySequence<T>*>(other);
        if (!otherLazy) {
            throw std::logic_error("LazySequence::Concat: other must be LazySequence");
        }

        return new LazySequence<T>(std::make_shared<LazySequence<T>>(*this), std::make_shared<LazySequence<T>>(*otherLazy));
    }

    Cardinal GetCardinalCount() const {
        Ordinal ord = GetOrderType();

        if (Ordinal::IsFinite(ord)) {
            return Cardinal::Finite(Ordinal::GetFinitePart(ord));
        }

        return Cardinal::CountablyInfinite();
    }

    Ordinal GetOrderType() const {
        if (IsConcat()) {
            return Ordinal::Add(left->GetOrderType(), right->GetOrderType());
        }

        return orderType;
    }

    size_t GetMaterializedCount() const {
        if (IsConcat()) {
            return 0;
        }
        return static_cast<size_t>(materialized.GetLength());
    }

    Enumerator<T> GetEnumerator() const override {
        return Enumerator<T>(new LazyEnumerator(this));
    }
};

#endif