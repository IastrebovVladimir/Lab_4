#ifndef LAZY_MENU_H
#define LAZY_MENU_H

#include <iostream>

#include "../Sequence/types.h"
#include "../LazySequence/LazySequence.h"

template <class T>
void run_lazy_menu(const TypeInfo* t);

template <class T>
void case_create_sequence(LazySequence<T>*& sequence, bool& sequenceBuilt, const TypeInfo* t);

template <class T>
void case_append_element(LazySequence<T>*& sequence, bool sequenceBuilt, const TypeInfo* t);

template <class T>
void case_prepend_element(LazySequence<T>*& sequence, bool sequenceBuilt, const TypeInfo* t);

template <class T>
void case_insert_element(LazySequence<T>*& sequence, bool sequenceBuilt, const TypeInfo* t);

template <class T>
void case_concat_sequence(LazySequence<T>*& sequence, bool sequenceBuilt, const TypeInfo* t);

template <class T>
void case_get_by_index(LazySequence<T>* sequence, bool sequenceBuilt, const TypeInfo* t);

template <class T>
void case_show_info(LazySequence<T>* sequence, bool sequenceBuilt);

template <class T>
void case_clear_sequence(LazySequence<T>*& sequence, bool& sequenceBuilt);

#include "menu_lazy.hpp"

#endif
