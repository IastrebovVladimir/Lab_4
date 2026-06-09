#include <iostream>
#include "../Sequence/types.h"
#include "menu_lazy.h"

int main() {
    int choice = 0;

    std::cout << "Choose value type for LazySequence:\n";
    std::cout << "1. int\n";
    std::cout << "2. double\n";
    std::cout << "Enter choice: ";

    while (!get_int_t()->scan(&choice) || choice < 1 || choice > 3) {
        std::cout << "Error! Enter an integer from 1 to 2: ";
    }

    switch (choice) {
        case 1:
            run_lazy_menu<int>(get_int_t());
            break;

        case 2:
            run_lazy_menu<double>(get_double_t());
            break;
    }

    return 0;
}