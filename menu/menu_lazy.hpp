#ifndef LAZY_MENU_HPP
#define LAZY_MENU_HPP

#include <iostream>
#include <stdexcept>

#include "../Sequence/types.h"
#include "../Sequence/ArraySequence.h"
#include "../LazySequence/LazySequence.h"


static Option<int> NaturalsRule(const MutableArraySequence<int>& m) {
    return Option<int>(m.GetLength() + 1);
}

static Option<int> OmegaTimes10Rule(const MutableArraySequence<int>& prefix) {
    return Option<int>((prefix.GetLength() + 1) * 10);
}

static Option<int> PowersOfTwoRule(const MutableArraySequence<int>& m) {
    if (m.GetLength() == 0) {
        return Option<int>();
    }
    return Option<int>(m.GetLast() * 2);
}

static Option<int> FibRule(const MutableArraySequence<int>& m) {
    if (m.GetLength() < 2) {
        return Option<int>();
    }
    int a = m.Get(m.GetLength() - 1);
    int b = m.Get(m.GetLength() - 2);
    return Option<int>(a + b);
}

static Option<double> DoublePlusPointOneRule(const MutableArraySequence<double>& m) {
    if (m.GetLength() == 0) {
        return Option<double>();
    }
    return Option<double>(m.GetLast() + 0.1);
}

static Option<double> DoublePowerTwoPointOneRule(const MutableArraySequence<double>& m) {
    if (m.GetLength() == 0) {
        return Option<double>();
    }
    return Option<double>(m.GetLast() * 2.1);
}

static Option<double> DoubleFibRule(const MutableArraySequence<double>& m) {
    if (m.GetLength() < 2) {
        return Option<double>();
    }
    double a = m.Get(m.GetLength() - 1);
    double b = m.Get(m.GetLength() - 2);
    return Option<double>(a + b);
}

#include "LazyMenuTraits.h"

template <typename T>
void run_lazy_menu(const TypeInfo* t) {
    LazySequence<T>* sequence = nullptr;
    bool sequenceBuilt = false;

    std::cout << "\nLazySequence menu\n";
    std::cout << "1. Create sequence\n";
    std::cout << "2. Append element\n";
    std::cout << "3. Prepend element\n";
    std::cout << "4. Insert element at index\n";
    std::cout << "5. Concat with another sequence\n";
    std::cout << "6. Get element by index\n";
    std::cout << "7. Show sequence info\n";
    std::cout << "8. Clear sequence\n";
    std::cout << "0. Exit\n";

    while (true) {
        int command = 0;
        std::cout << "Choose: ";

        while (!get_int_t()->scan(&command) || command < 0 || command > 8) {
            std::cout << "Error! Enter an integer from 0 to 8: ";
        }

        switch (command) {
            case 1:
                case_create_sequence(sequence, sequenceBuilt, t);
                break;

            case 2:
                case_append_element(sequence, sequenceBuilt, t);
                break;

            case 3:
                case_prepend_element(sequence, sequenceBuilt, t);
                break;

            case 4:
                case_insert_element(sequence, sequenceBuilt, t);
                break;

            case 5:
                case_concat_sequence(sequence, sequenceBuilt, t);
                break;

            case 6:
                case_get_by_index(sequence, sequenceBuilt, t);
                break;

            case 7:
                case_show_info(sequence, sequenceBuilt);
                break;

            case 8:
                case_clear_sequence(sequence, sequenceBuilt);
                break;

            case 0:
                delete sequence;
                std::cout << "Exit.\n";
                return;

            default:
                std::cout << "Unknown command.\n";
        }
    }
}

template <class T>
void case_create_sequence(LazySequence<T>*& sequence, bool& sequenceBuilt, const TypeInfo* t) {
    if (sequence != nullptr) {
        delete sequence;
        sequence = nullptr;
    }

    sequenceBuilt = false;

    int createCommand = 0;

    std::cout << "\nCreate sequence\n";
    std::cout << "1. Create empty sequence\n";
    std::cout << "2. Create finite sequence\n";
    std::cout << "3. Create infinite sequence\n";
    std::cout << "0. Back\n";
    std::cout << "Choose: ";

    while (!get_int_t()->scan(&createCommand) || createCommand < 0 || createCommand > 3) {
        std::cout << "Error! Enter an integer from 0 to 3: ";
    }

    if (createCommand == 0) {
        return;
    }

    if (createCommand == 1) {
        sequence = new LazySequence<T>();
        sequenceBuilt = true;
        std::cout << "Empty sequence created.\n";
        return;
    }

    if (createCommand == 2) {
        int count = 0;
        std::cout << "Enter number of elements: ";

        while (!get_int_t()->scan(&count) || count < 0) {
            std::cout << "Error! Enter integer >= 0: ";
        }

        MutableArraySequence<T> items;

        for (int index = 0; index < count; index++) {
            T value;
            std::cout << "Enter element " << index << ": ";

            while (!t->scan(&value)) {
                std::cout << "Error! Enter correct value: ";
            }

            items.Append(value);
        }

        sequence = new LazySequence<T>(items, nullptr, false);
        sequenceBuilt = true;
        std::cout << "Finite sequence created.\n";
        return;
    }

    if (createCommand == 3) {
        int ruleCommand = 0;

        std::cout << "\nChoose infinite sequence rule\n";
        LazyMenuTraits<T>::print_infinite_menu();

        while (!get_int_t()->scan(&ruleCommand) || ruleCommand < 0 || ruleCommand > 3) {
            std::cout << "Error! Enter an integer from 0 to 3: ";
        }

        if (ruleCommand == 0) {
            return;
        }

        MutableArraySequence<T> prefix = LazyMenuTraits<T>::build_prefix(ruleCommand);

        auto rule =
                (ruleCommand == 1) ? LazyMenuTraits<T>::rule1() :
                (ruleCommand == 2) ? LazyMenuTraits<T>::rule2() :
                LazyMenuTraits<T>::rule3();

        sequence = new LazySequence<T>(rule, prefix, true);
        sequenceBuilt = true;
        std::cout << "Infinite sequence created.\n";
        return;
    }
}

template <typename T>
void case_append_element(LazySequence<T>*& sequence, bool sequenceBuilt, const TypeInfo* t) {
    if (!sequenceBuilt || sequence == nullptr) {
        std::cout << "Sequence is not created.\n";
        return;
    }

    T value;
    std::cout << "Enter element to append: ";

    while (!t->scan(&value)) {
        std::cout << "Error! Enter correct value: ";
    }

    Sequence<T>* appended = sequence->Append(value);
    LazySequence<T>* newSequence = dynamic_cast<LazySequence<T>*>(appended);

    if (newSequence == nullptr) {
        delete appended;
        std::cout << "Append failed.\n";
        return;
    }

    delete sequence;
    sequence = newSequence;
    std::cout << "Element appended.\n";
}

template <class T>
void case_prepend_element(LazySequence<T>*& sequence, bool sequenceBuilt, const TypeInfo* t) {
    if (!sequenceBuilt || sequence == nullptr) {
        std::cout << "Sequence is not created.\n";
        return;
    }

    T value;
    std::cout << "Enter element to prepend: ";

    while (!t->scan(&value)) {
        std::cout << "Error! Enter correct value: ";
    }

    Sequence<T>* prepended = sequence->Prepend(value);
    LazySequence<T>* newSequence = dynamic_cast<LazySequence<T>*>(prepended);

    if (newSequence == nullptr) {
        delete prepended;
        std::cout << "Prepend failed.\n";
        return;
    }

    delete sequence;
    sequence = newSequence;
    std::cout << "Element prepended.\n";
}

template <typename T>
void case_insert_element(LazySequence<T>*& sequence, bool sequenceBuilt, const TypeInfo* t) {
    if (!sequenceBuilt || sequence == nullptr) {
        std::cout << "Sequence is not created.\n";
        return;
    }

    T value;
    int index = 0;

    std::cout << "Enter element to insert: ";
    while (!t->scan(&value)) {
        std::cout << "Error! Enter correct value: ";
    }

    std::cout << "Enter index: ";
    while (!get_int_t()->scan(&index) || index < 0) {
        std::cout << "Error! Enter integer >= 0: ";
    }

    try {
        Sequence<T>* inserted = sequence->InsertAt(value, index);
        LazySequence<T>* newSequence = dynamic_cast<LazySequence<T>*>(inserted);

        if (newSequence == nullptr) {
            delete inserted;
            std::cout << "Insert failed.\n";
            return;
        }

        delete sequence;
        sequence = newSequence;
        std::cout << "Element inserted.\n";
    } catch (const std::exception& error) {
        std::cout << "Error: " << error.what() << '\n';
    }
}

template <class T>
void case_concat_sequence(LazySequence<T>*& sequence, bool sequenceBuilt, const TypeInfo* t) {
    if (!sequenceBuilt || sequence == nullptr) {
        std::cout << "Sequence is not created.\n";
        return;
    }

    int concatCommand = 0;

    std::cout << "\nConcat with another sequence\n";
    std::cout << "1. Concat with finite sequence\n";
    std::cout << "2. Concat with infinite sequence\n";
    std::cout << "0. Back\n";
    std::cout << "Choose: ";

    while (!get_int_t()->scan(&concatCommand) || concatCommand < 0 || concatCommand > 2) {
        std::cout << "Error! Enter an integer from 0 to 2: ";
    }

    if (concatCommand == 0) {
        return;
    }

    LazySequence<T>* otherSequence = nullptr;

    if (concatCommand == 1) {
        int count = 0;
        std::cout << "Enter number of elements: ";

        while (!get_int_t()->scan(&count) || count < 0) {
            std::cout << "Error! Enter integer >= 0: ";
        }

        MutableArraySequence<T> items;

        for (int index = 0; index < count; index++) {
            T value;
            std::cout << "Enter element " << index << ": ";

            while (!t->scan(&value)) {
                std::cout << "Error! Enter correct value: ";
            }

            items.Append(value);
        }

        otherSequence = new LazySequence<T>(items, nullptr, false);
    }

    if (concatCommand == 2) {
        int ruleCommand = 0;

        std::cout << "\nChoose infinite sequence rule\n";
        LazyMenuTraits<T>::print_infinite_menu();

        while (!get_int_t()->scan(&ruleCommand) || ruleCommand < 0 || ruleCommand > 3) {
            std::cout << "Error! Enter an integer from 0 to 3: ";
        }

        if (ruleCommand == 0) {
            return;
        }

        MutableArraySequence<T> prefix = LazyMenuTraits<T>::build_prefix(ruleCommand);

        auto rule =
                (ruleCommand == 1) ? LazyMenuTraits<T>::rule1() :
                (ruleCommand == 2) ? LazyMenuTraits<T>::rule2() :
                LazyMenuTraits<T>::rule3();

        otherSequence = new LazySequence<T>(rule, prefix, true);
    }

    if (otherSequence == nullptr) {
        std::cout << "Concat failed: second sequence was not created.\n";
        return;
    }

    try {
        Sequence<T>* concatenated = sequence->Concat(otherSequence);
        LazySequence<T>* newSequence = dynamic_cast<LazySequence<T>*>(concatenated);

        delete otherSequence;

        if (newSequence == nullptr) {
            delete concatenated;
            std::cout << "Concat failed.\n";
            return;
        }

        delete sequence;
        sequence = newSequence;
        std::cout << "Sequence concatenated.\n";
    } catch (const std::exception& error) {
        delete otherSequence;
        std::cout << "Error: " << error.what() << '\n';
    }
}

template <typename T>
void case_get_by_index(LazySequence<T>* sequence, bool sequenceBuilt, const TypeInfo* t) {
    if (!sequenceBuilt || sequence == nullptr) {
        std::cout << "Sequence is not created.\n";
        return;
    }

    int index = 0;
    std::cout << "Enter index: ";

    while (!get_int_t()->scan(&index) || index < 0) {
        std::cout << "Error! Enter integer >= 0: ";
    }

    try {
        const T& value = sequence->Get(index);
        std::cout << "Element at index " << index << ": ";
        t->print(&value);
        std::cout << '\n';
    } catch (const std::exception& error) {
        std::cout << "Error: " << error.what() << '\n';
    }
}

template <typename T>
void case_show_info(LazySequence<T>* sequence, bool sequenceBuilt) {
    if (!sequenceBuilt || sequence == nullptr) {
        std::cout << "Sequence is not created.\n";
        return;
    }

    std::cout << "Sequence info:\n";
    std::cout << "Materialized elements: " << sequence->GetMaterializedCount() << '\n';

    try {
        std::cout << "Length: " << sequence->GetLength() << '\n';
    } catch (const std::exception&) {
        std::cout << "Length: infinite or non-finite\n";
    }

    try {
        Cardinal cardinal = sequence->GetCardinalCount();
        if (cardinal.isCountablyInfinite) {
            std::cout << "Cardinality: infinite\n";
        } else {
            std::cout << "Cardinality: " << cardinal.finiteValue << '\n';
        }
    } catch (const std::exception&) {
        std::cout << "Cardinality: unknown\n";
    }
}

template <typename T>
void case_clear_sequence(LazySequence<T>*& sequence, bool& sequenceBuilt) {
    if (sequence != nullptr) {
        delete sequence;
        sequence = nullptr;
    }

    sequenceBuilt = false;
    std::cout << "Sequence cleared.\n";
}

#endif
