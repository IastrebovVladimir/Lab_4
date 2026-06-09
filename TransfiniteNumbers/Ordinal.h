#ifndef ORDINAL_H
#define ORDINAL_H

#include <cstddef>
#include <stdexcept>

struct Ordinal {
    size_t omegaCoeff;
    size_t finitePart;

    Ordinal(size_t omega = 0, size_t finite = 0) : omegaCoeff(omega), finitePart(finite) {}

    static Ordinal MakeFinite(size_t value) {
        return Ordinal(0, value);
    }
    static Ordinal MakeOmegaCoeff(size_t coeff) {
        return Ordinal(coeff, 0);
    }

    static Ordinal MakeOmega() {
        return Ordinal(1, 0);
    }

    static bool IsZero(const Ordinal& value) {
        return value.omegaCoeff == 0 && value.finitePart == 0;
    }

    static bool IsFinite(const Ordinal& value) {
        return value.omegaCoeff == 0;
    }

    static size_t GetFinitePart(const Ordinal& value) {
        if (!IsFinite(value)) {
            throw std::out_of_range("Ordinal is not finite");
        }
        return value.finitePart;
    }

    static int Compare(const Ordinal& left, const Ordinal& right) {
        if (left.omegaCoeff < right.omegaCoeff)
            return -1;
        if (left.omegaCoeff > right.omegaCoeff)
            return 1;
        if (left.finitePart < right.finitePart)
            return -1;
        if (left.finitePart > right.finitePart)
            return 1;
        return 0;
    }

    static Ordinal Add(const Ordinal& left, const Ordinal& right) {
        if (right.omegaCoeff > 0) {
            return Ordinal(left.omegaCoeff + right.omegaCoeff, right.finitePart);
        }
        return Ordinal(left.omegaCoeff, left.finitePart + right.finitePart);
    }

    static bool TryRemovePrefix(const Ordinal& full, const Ordinal& prefix, Ordinal& remainder) {
        if (prefix.omegaCoeff > full.omegaCoeff) {
            return false;
        }

        if (prefix.omegaCoeff < full.omegaCoeff) {
            remainder = Ordinal(full.omegaCoeff - prefix.omegaCoeff, full.finitePart);
            return true;
        }

        if (prefix.finitePart > full.finitePart) {
            return false;
        }

        remainder = Ordinal(0, full.finitePart - prefix.finitePart);
        return true;
    }
};

#endif