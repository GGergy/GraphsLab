//
// Created by gergy on 27.10.2026.
//
#include <stdio.h>
#include <stdlib.h>

#include "set.h"


set_t *create_set(const int name) {
    // Создание головы множества

    set_t *set = malloc(sizeof(set_t));
    set->edge.from = name;
    set->edge.to = name;
    set->edge.weight = 0;

    set->next = nullptr;
    return set;
}


void destroy_set(set_t *set) {
    // Освобождение памяти от множества

    set_t *cur = set;
    while (cur != nullptr) {
        set_t *tmp = cur->next;
        free(cur);
        cur = tmp;
    }
}


bool insert(set_t *set, const int to, const int weight) {
    // Добавление элемента со значением val во множество set с сохранением порядка
    // return true - успешно, return false - такой элемент уже есть в этом множестве

    set_t *parent = set;
    while (parent->next != nullptr) {
        if (parent->next->edge.to == to && parent->next->edge.from == set->edge.from) {
            return false;
        }
        if (parent->next->edge.to > to) {
            break;
        }
        parent = parent->next;
    }
    set_t *node = malloc(sizeof(set_t));
    node->next = parent->next;
    parent->next = node;

    set->edge.weight++;
    node->edge.from = set->edge.from;
    node->edge.to = to;
    node->edge.weight = weight;

    return true;
}


bool _insrt(set_t *set, const int from, const int to, const int weight) {

    set_t *parent = set;
    while (parent->next != nullptr) {
        if (parent->next->edge.to == to && parent->next->edge.from == from) {
            return false;
        }
        if (parent->next->edge.to > to) {
            break;
        }
        parent = parent->next;
    }
    set_t *node = malloc(sizeof(set_t));
    node->next = parent->next;
    parent->next = node;

    set->edge.weight++;
    node->edge.from = from;
    node->edge.to = to;
    node->edge.weight = weight;

    return true;
}


bool delete(set_t *set, const int to) {
    // Поиск и удаление элемента со значением val из множества set
    // return true - успешно, return false - элемента нет в этом множестве

    set_t *parent = set;
    while (parent->next != nullptr) {
        if (parent->next->edge.to > to) return -1;
        if (parent->next->edge.to == to) {
            set_t *tmp = parent->next->next;
            free(parent->next);
            parent->next = tmp;
            set->edge.weight--;
            return true;
        }
        parent = parent->next;
    }
    return false;
}


edge_t *find_edge(const set_t *set, const int to) {
    const set_t *parent = set;
    while (parent->next != nullptr) {
        if (parent->next->edge.to == to) {
            return &parent->next->edge;
        }
        if (parent->next->edge.to > to) {
            break;
        }
        parent = parent->next;
    }
    return nullptr;
}

size_t set_size(const set_t *set) {
    // Размер множества

    const set_t *cur = set->next;
    size_t size = 0;
    while (cur != nullptr) {
        size++;
        cur = cur->next;
    }
    return size;
}


void print_set(const set_t *set) {

    const set_t *cur = set;
    putchar('{');
    if (cur == nullptr) {
        putchar('}');
        return;
    }
    printf("[%d->%d, w:%d]", cur->edge.from, cur->edge.to, cur->edge.weight);
    cur = cur->next;

    while (cur != nullptr) {
        printf(", [%d->%d, w:%d]", cur->edge.from, cur->edge.to, cur->edge.weight);
        cur = cur->next;
    }
    putchar('}');
}
