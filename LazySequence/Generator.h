#ifndef GENERATOR_H
#define GENERATOR_H

#include <memory>
#include <stdexcept>
#include "../Sequence/ArraySequence.h"

template <typename T>
class Generator {
public:
    virtual bool HasNext(const MutableArraySequence<T>& materialized) const = 0;
    virtual T GetNext(const MutableArraySequence<T>& materialized) = 0;
    virtual std::shared_ptr<Generator<T>> Clone() const = 0;

    virtual Option<T> TryGetNext(const MutableArraySequence<T>& materialized) {
        if (!HasNext(materialized)) {
            return Option<T>();
        }
        return Option<T>(GetNext(materialized));
    }

    virtual ~Generator() = default;
};

template <typename T>
class EmptyGenerator : public Generator<T> {
public:
    bool HasNext(const MutableArraySequence<T>&) const override {
        return false;
    }

    T GetNext(const MutableArraySequence<T>&) override {
        throw std::out_of_range("EmptyGenerator: no next element");
    }

    Option<T> TryGetNext(const MutableArraySequence<T>&) override {
        return Option<T>();
    }

    std::shared_ptr<Generator<T>> Clone() const override {
        return std::make_shared<EmptyGenerator<T>>();
    }
};

template <typename T, typename Rule>
class RuleGenerator : public Generator<T> {
private:
    Rule rule;

public:
    explicit RuleGenerator(Rule ruleFunction)
            : rule(ruleFunction) {}

    bool HasNext(const MutableArraySequence<T>& materialized) const override {
        Option<T> result = rule(materialized);
        return result.IsSome();
    }

    T GetNext(const MutableArraySequence<T>& materialized) override {
        Option<T> result = rule(materialized);
        if (result.IsNone()) {
            throw std::out_of_range("RuleGenerator: no next element");
        }
        return result.GetValue();
    }

    Option<T> TryGetNext(const MutableArraySequence<T>& materialized) override {
        return rule(materialized);
    }

    std::shared_ptr<Generator<T>> Clone() const override {
        return std::make_shared<RuleGenerator<T, Rule>>(rule);
    }
};

enum class OperationType {
    AppendOne,
    PrependOne,
    InsertOne
};

template <typename T>
class OperationGenerator : public Generator<T> {
protected:
    std::shared_ptr<Generator<T>> source;

    MutableArraySequence<T> CopyRange(const MutableArraySequence<T>& input, int left, int right) const {
        MutableArraySequence<T> result;
        for (int i = left; i < right; ++i) {
            result.Append(input.Get(i));
        }
        return result;
    }
    MutableArraySequence<T> CopyWithoutIndex(const MutableArraySequence<T>& input, int removedIndex) const {
        if (removedIndex < 0 || removedIndex >= input.GetLength())
            throw std::out_of_range("OperationGenerator: remove index out of range");
        MutableArraySequence<T> result;
        for (int i = 0; i < input.GetLength(); i++) {
            if (i != removedIndex)
                result.Append(input.Get(i));
        }
        return result;
    }

public:
    explicit OperationGenerator(std::shared_ptr<Generator<T>> sourceGenerator)
            : source(std::move(sourceGenerator)) {}

    virtual ~OperationGenerator() = default;
};

template <typename T>
class AppendGenerator : public OperationGenerator<T> {
private:
    T item;

    bool EndsWithAppended(const MutableArraySequence<T>& resultPrefix) const {
        if (resultPrefix.GetLength() == 0) {
            return false;
        }

        if (!(resultPrefix.Get(resultPrefix.GetLength() - 1) == item)) {
            return false;
        }

        MutableArraySequence<T> withoutLast = this->CopyWithoutIndex(
                resultPrefix,
                resultPrefix.GetLength() - 1
        );

        return this->source && !this->source->HasNext(withoutLast);
    }

public:
    AppendGenerator(std::shared_ptr<Generator<T>> sourceGenerator, const T& appendedItem)
            : OperationGenerator<T>(std::move(sourceGenerator)), item(appendedItem) {}

    bool HasNext(const MutableArraySequence<T>& resultPrefix) const override {
        if (this->source && this->source->HasNext(resultPrefix)) {
            return true;
        }

        if (EndsWithAppended(resultPrefix)) {
            return false;
        }

        return true;
    }

    T GetNext(const MutableArraySequence<T>& resultPrefix) override {
        if (this->source && this->source->HasNext(resultPrefix)) {
            return this->source->GetNext(resultPrefix);
        }

        if (EndsWithAppended(resultPrefix)) {
            throw std::out_of_range("AppendGenerator: no next element");
        }

        return item;
    }

    Option<T> TryGetNext(const MutableArraySequence<T>& resultPrefix) override {
        if (!HasNext(resultPrefix)) {
            return Option<T>();
        }
        return Option<T>(GetNext(resultPrefix));
    }

    std::shared_ptr<Generator<T>> Clone() const override {
        return std::make_shared<AppendGenerator<T>>(
                this->source ? this->source->Clone() : nullptr,
                item
        );
    }
};

template <typename T>
class PrependGenerator : public OperationGenerator<T> {
private:
    T item;

public:
    PrependGenerator(std::shared_ptr<Generator<T>> sourceGenerator, const T& prependedItem)
            : OperationGenerator<T>(std::move(sourceGenerator)), item(prependedItem) {}

    bool HasNext(const MutableArraySequence<T>& resultPrefix) const override {
        if (resultPrefix.GetLength() == 0) {
            return true;
        }

        MutableArraySequence<T> sourcePrefix = this->CopyRange(resultPrefix, 1, resultPrefix.GetLength());
        return this->source && this->source->HasNext(sourcePrefix);
    }

    T GetNext(const MutableArraySequence<T>& resultPrefix) override {
        if (resultPrefix.GetLength() == 0) {
            return item;
        }

        MutableArraySequence<T> sourcePrefix = this->CopyRange(resultPrefix, 1, resultPrefix.GetLength());
        if (this->source && this->source->HasNext(sourcePrefix)) {
            return this->source->GetNext(sourcePrefix);
        }

        throw std::out_of_range("PrependGenerator: no next element");
    }

    std::shared_ptr<Generator<T>> Clone() const override {
        return std::make_shared<PrependGenerator<T>>(this->source ? this->source->Clone() : nullptr, item);
    }
};

template <typename T>
class InsertGenerator : public OperationGenerator<T> {
private:
    T item;
    int insertIndex;

    bool InsertedAlready(const MutableArraySequence<T>& resultPrefix) const {
        return resultPrefix.GetLength() > insertIndex;
    }

public:
    InsertGenerator(std::shared_ptr<Generator<T>> sourceGenerator, const T& insertedItem, int index)
            : OperationGenerator<T>(std::move(sourceGenerator)),
              item(insertedItem),
              insertIndex(index) {
        if (index < 0) {
            throw std::out_of_range("InsertGenerator: negative index");
        }
    }

    bool HasNext(const MutableArraySequence<T>& resultPrefix) const override {
        if (resultPrefix.GetLength() == insertIndex) {
            return true;
        }

        MutableArraySequence<T> sourcePrefix;
        if (resultPrefix.GetLength() <= insertIndex) {
            sourcePrefix = resultPrefix;
        } else {
            sourcePrefix = this->CopyRange(resultPrefix, 0, insertIndex);
            for (int i = insertIndex + 1; i < resultPrefix.GetLength(); ++i) {
                sourcePrefix.Append(resultPrefix.Get(i));
            }
        }

        return this->source && this->source->HasNext(sourcePrefix);
    }

    T GetNext(const MutableArraySequence<T>& resultPrefix) override {
        if (resultPrefix.GetLength() == insertIndex) {
            return item;
        }

        MutableArraySequence<T> sourcePrefix;
        if (resultPrefix.GetLength() <= insertIndex) {
            sourcePrefix = resultPrefix;
        } else {
            sourcePrefix = this->CopyRange(resultPrefix, 0, insertIndex);
            for (int i = insertIndex + 1; i < resultPrefix.GetLength(); ++i) {
                sourcePrefix.Append(resultPrefix.Get(i));
            }
        }

        if (this->source && this->source->HasNext(sourcePrefix)) {
            return this->source->GetNext(sourcePrefix);
        }

        throw std::out_of_range("InsertGenerator: no next element");
    }

    std::shared_ptr<Generator<T>> Clone() const override {
        return std::make_shared<InsertGenerator<T>>(this->source ? this->source->Clone() : nullptr, item, insertIndex);
    }
};

#endif
