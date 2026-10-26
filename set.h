// Реализация функционала множеств

#pragma once
#include <stdio.h>
#include <stdlib.h>

#ifndef SET_H
#define SET_H


typedef struct set {
    char val;
    struct set *next;
} set_t;


inline set_t *create_set(char name);
inline void destroy_set(set_t *set);
inline int insert(set_t *set, char val);
inline int delete(set_t *set, char val);
inline void set_print(const set_t *set);
inline int set_size(const set_t *set);
inline set_t *append_node(set_t *parent, char val);


set_t *create_set(const char name) {
    // Создание головы множества

    set_t *set = malloc(sizeof(set_t));
    set->val = name;
    set->next = NULL;
    return set;
}


void destroy_set(set_t *set) {
    // Освобождение памяти от множества

    set_t *cur = set;
    while (cur != NULL) {
        set_t *tmp = cur->next;
        free(cur);
        cur = tmp;
    }
}


int insert(set_t *set, const char val) {
    // Добавление элемента со значением val во множество set с сохранением порядка
    // return 0 - успешно, return -1 - такой элемент уже есть в этом множестве

    set_t *parent = set;
    while (parent->next != NULL) {
        if (parent->next->val == val) {
            return -1;
        }
        if (parent->next->val > val) {
            break;
        }
        parent = parent->next;
    }
    set_t *node = malloc(sizeof(set_t));
    node->next = parent->next;
    parent->next = node;
    node->val = val;

    return 0;
}


int delete(set_t *set, const char val) {
    // Поиск и удаление элемента со значением val из множества set
    // return 0 - успешно, return -1 - элемента нет в этом множестве

    set_t *parent = set;
    while (parent->next != NULL) {
        if (parent->next->val > val) return -1;
        if (parent->next->val == val) {
            set_t *tmp = parent->next->next;
            free(parent->next);
            parent->next = tmp;
            return 0;
        }
        parent = parent->next;
    }
    return -1;
}

void set_print(const set_t *set) {
    // Обычная печать множества

    const set_t *cur = set->next;

    putchar('{');
    if (cur == NULL) {
        putchar('}');
        return;
    }
    putchar(cur->val);
    cur = cur->next;

    while (cur != NULL) {
        printf(", %c", cur->val);
        cur = cur->next;
    }

    putchar('}');


}

int set_size(const set_t *set) {
    // Размер множества

    const set_t *cur = set->next;
    int size = 0;
    while (cur != NULL) {
        size++;
        cur = cur->next;
    }
    return size;
}

set_t *append_node(set_t *parent, const char val) {
    // Добавление ноды к родителю

    set_t *node = malloc(sizeof(set_t));
    node->val = val;
    node->next = NULL;
    parent->next = node;

    return node;
}


#endif
