#ifndef CARDINAL_H
#define CARDINAL_H

#include <cstddef>

struct Cardinal {
    bool isCountablyInfinite = false;
    size_t finiteValue = 0;

    static Cardinal Finite(size_t value) {
        return {false, value};
    }

    static Cardinal CountablyInfinite() {
        return {true, 0};
    }
};

#endif
