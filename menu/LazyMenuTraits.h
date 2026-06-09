#ifndef LAZYMENUTRAITS_H
#define LAZYMENUTRAITS_H

template <class T>
struct LazyMenuTraits;

template <>
struct LazyMenuTraits<int> {
    static void print_infinite_menu() {
        std::cout << "1. Natural numbers: 1, 2, 3, ...\n";
        std::cout << "2. Powers of two: start, start*2, start*4, ...\n";
        std::cout << "3. Fibonacci numbers\n";
        std::cout << "0. Back\n";
        std::cout << "Choose: ";
    }

    static auto rule1() {
        return NaturalsRule;
    }

    static auto rule2() {
        return OmegaTimes10Rule;
    }

    static auto rule3() {
        return FibRule;
    }

    static MutableArraySequence<int> build_prefix(int ruleCommand) {
        MutableArraySequence<int> prefix;

        if (ruleCommand == 3) {
            prefix.Append(1);
            prefix.Append(1);
        }

        return prefix;
    }
};

template <>
struct LazyMenuTraits<double> {
    static void print_infinite_menu() {
        std::cout << "1. Sequence: 1.0, 1.1, 1.2, ...\n";
        std::cout << "2. Powers of 2.1: start, start*2.1, start*2.1^2, ...\n";
        std::cout << "3. Fibonacci-like sequence\n";
        std::cout << "0. Back\n";
        std::cout << "Choose: ";
    }

    static auto rule1() {
        return DoublePlusPointOneRule;
    }

    static auto rule2() {
        return DoublePowerTwoPointOneRule;
    }

    static auto rule3() {
        return DoubleFibRule;
    }

    static MutableArraySequence<double> build_prefix(int ruleCommand) {
        MutableArraySequence<double> prefix;

        if (ruleCommand == 1) {
            prefix.Append(1.0);
        }

        if (ruleCommand == 3) {
            prefix.Append(1.0);
            prefix.Append(1.1);
        }

        return prefix;
    }
};

#endif
