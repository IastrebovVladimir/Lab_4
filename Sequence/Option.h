#ifndef OPTION_H
#define OPTION_H

#include <stdexcept>

template <class T>
class Option {
private:
    T value;
    bool hasValue;
public:
    Option() : hasValue(false) {}
    Option(const T& val) : value(val), hasValue(true) {}

    bool IsSome() const {
        return hasValue;
    }
    bool IsNone() const {
        return !hasValue;
    }
    const T &GetValue() const {
        if (!hasValue) {
            throw std::logic_error("Option has no value");
        }

        return value;
    }
};

#endif
