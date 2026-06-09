#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stdexcept>
#include "IEnumerator.h"

template <typename T>
class LinkedList {
private:
    struct ListItem {
        T data;
        ListItem* next;

        ListItem(const T& value, ListItem* nextPtr = nullptr)
                : data(value), next(nextPtr) {}

        ListItem(const ListItem& other)
                : data(other.data), next(nullptr) {}
    };

    ListItem* head;
    ListItem* tail;
    int length;

    ListItem* GetItem(int index) const {
        if (index < 0 || index >= length) {
            throw std::out_of_range("Index out of range in ListItem::Get");
        }

        ListItem* current = head;
        for (int i = 0; i < index; ++i) {
            current = current->next;
        }
        return current;
    }

    class LinkedListEnumerator : public IEnumerator<T> {
    private:
        const ListItem* current;
        bool beforeStart;

    public:
        explicit LinkedListEnumerator(const ListItem* start)
                : current(start), beforeStart(true) {}

        bool MoveNext() override {
            if (beforeStart) {
                beforeStart = false;
            } else if (current != nullptr) {
                current = current->next;
            }
            return current != nullptr;
        }

        const T& Current() const override {
            if (current == nullptr) {
                throw std::out_of_range("LinkedListEnumerator::Current: invalid position");
            }
            return current->data;
        }
    };

public:
    LinkedList() : head(nullptr), tail(nullptr), length(0) {}

    LinkedList(T* items, int count) : head(nullptr), tail(nullptr), length(0) {
        for (int index = 0; index < count; ++index) {
            Append(items[index]);
        }
    }

    LinkedList(const LinkedList<T>& linkedList)
            : head(nullptr), tail(nullptr), length(0) {
        ListItem* current = linkedList.head;
        while (current != nullptr) {
            Append(current->data);
            current = current->next;
        }
    }

    Enumerator<T> GetEnumerator() const {
        return Enumerator<T>(new LinkedListEnumerator(head));
    }

    const T& Get(int index) const {
        if (index < 0 || index >= length) {
            throw std::out_of_range("Index out of range in LinkedList::Get");
        }

        ListItem* current = head;
        int element_index = 0;
        while (element_index < index) {
            current = current->next;
            element_index++;
        }

        return current->data;
    }

    const T& GetFirst() const {
        if (head == nullptr) {
            throw std::out_of_range("List is empty");
        }
        return head->data;
    }

    const T& GetLast() const {
        if (tail == nullptr) {
            throw std::out_of_range("List is empty");
        }
        return tail->data;
    }

    int GetLength() const {
        return length;
    }

    void Append(const T& item) {
        ListItem* newItem = new ListItem(item);

        if (head == nullptr) {
            head = tail = newItem;
        } else {
            tail->next = newItem;
            tail = newItem;
        }

        length++;
    }

    void Prepend(const T& item) {
        ListItem* newItem = new ListItem(item);

        if (head == nullptr) {
            head = tail = newItem;
        } else {
            newItem->next = head;
            head = newItem;
        }

        length++;
    }

    void InsertAt(const T& item, int index) {
        if (index < 0 || index > length) {
            throw std::out_of_range("Index out of range");
        }

        if (index == 0) {
            Prepend(item);
        } else if (index == length) {
            Append(item);
        } else {
            ListItem* newItem = new ListItem(item);
            ListItem* current = head;

            for (int i = 0; i < index - 1; ++i) {
                current = current->next;
            }

            newItem->next = current->next;
            current->next = newItem;
            length++;
        }
    }

    LinkedList<T>* GetSubList(int startIndex, int endIndex) const {
        if (startIndex < 0 || endIndex >= length || startIndex > endIndex) {
            throw std::out_of_range("Index out of range in GetSubList");
        }

        LinkedList<T>* subList = new LinkedList<T>();
        ListItem* current = GetItem(startIndex);

        for (int i = startIndex; i <= endIndex; ++i) {
            subList->Append(current->data);
            current = current->next;
        }

        return subList;
    }

    LinkedList<T>* Concat(const LinkedList<T>* conList) const {
        LinkedList<T>* newList = new LinkedList<T>(*this);

        ListItem* current = conList->head;
        while (current != nullptr) {
            newList->Append(current->data);
            current = current->next;
        }

        return newList;
    }

    ~LinkedList() {
        ListItem* current = head;

        while (current != nullptr) {
            ListItem* next = current->next;
            delete current;
            current = next;
        }

        length = 0;
        head = nullptr;
        tail = nullptr;
    }
};

#endif