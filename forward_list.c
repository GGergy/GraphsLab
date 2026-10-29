//
// Created by gergy on 27.10.2026.
//
#include <stdlib.h>
#include <stdio.h>

#include "forward_list.h"


inline list_t *create_list() {
    // Создание списка

    list_t *list = malloc(sizeof(list_t));
    list->size = 0;
    list->head = nullptr;
    return list;
}


inline void destroy_list(list_t *list) {
    // Освобождение памяти

    node_t *cur = list->head;
    while (cur != nullptr) {
        node_t *tmp = cur->next;
        // Освобождение множества из каждой ноды
        destroy_set(cur->set);
        free(cur);
        cur = tmp;
    }
    free(list);
}

inline set_t *add_set(list_t* list, const int set_name) {
    // Добавление новой ноды со значением головы множества без порядка

    set_t *set = create_set(set_name);
    node_t *node = malloc(sizeof(node_t));
    node->set = set;
    node->next = list->head;
    list->size++;
    list->head = node;

    return set;
}


inline set_t *find_set(const list_t *list, const int set_name) {
    // Поиск головы множества с именем set_name

    const node_t *cur = list->head;
    while (cur != nullptr) {
        if (cur->set->edge.from == set_name) {
            return cur->set;
        }
        cur = cur->next;
    }
    return nullptr;
}


inline bool delete_set(list_t *list, const int set_name) {
    // Удаление ноды с головой множества по имени set_name
    // return 0 - успешно, return -1 - не найдено

    node_t *parent = list->head;
    if (parent == nullptr) return false;
    if (parent->set->edge.from == set_name) {
        list->size--;
        list->head = parent->next;
        free(parent);
        return 0;
    }
    while (parent->next != nullptr) {
        if (parent->next->set->edge.from == set_name) {
            list->size--;
            node_t *tmp = parent->next->next;
            destroy_set(parent->next->set);
            free(parent->next);
            parent->next = tmp;
            return true;
        }
        parent = parent->next;
    }
    return false;
}


inline void print_sets_names(const list_t *list) {
    // Вывод имен всех множеств, головы которых хранятся в списке

    putchar('[');
    if (list->head) {
        putchar(list->head->set->edge.from);
        const node_t *cur = list->head->next;
        while (cur != nullptr) {
            printf(", %d", cur->set->edge.from);
            cur = cur->next;
        }
    }
    putchar(']');
}