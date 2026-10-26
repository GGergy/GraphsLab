// Реализация простого односвязного списка для хранения голов множеств

#pragma once
#include "set.h"

#ifndef FORWARD_LIST_H
#define FORWARD_LIST_H


typedef struct Node {
    set_t *set;
    struct Node *next;
} node_t;


typedef struct List {
    node_t *head;
    int size;
} list_t;


inline list_t *create_list();
inline void destroy_list(list_t *list);
inline node_t *fl_add_node(list_t* list, set_t *val);
inline set_t *find_set(const list_t *list, char set_name);
inline int fl_delete(list_t *list, char set_name);
inline void print_sets_names(const list_t *list);


list_t *create_list() {
    // Создание списка

    list_t *list = malloc(sizeof(list_t));
    list->size = 0;
    list->head = NULL;
    return list;
}


void destroy_list(list_t *list) {
    // Освобождение памяти

    node_t *cur = list->head;
    while (cur != NULL) {
        node_t *tmp = cur->next;
        // Освобождение множества из каждой ноды
        destroy_set(cur->set);
        free(cur);
        cur = tmp;
    }
    free(list);
}

node_t *fl_add_node(list_t* list, set_t *val) {
    // Добавление новой ноды со значением головы множества без порядка

    node_t *node = malloc(sizeof(node_t));
    node->set = val;
    node->next = list->head;
    list->size++;
    list->head = node;

    return node;
}


set_t *find_set(const list_t *list, const char set_name) {
    // Поиск головы множества с именем set_name

    const node_t *cur = list->head;
    while (cur != NULL) {
        if (cur->set->val == set_name) {
            return cur->set;
        }
        cur = cur->next;
    }
    return NULL;
}


int fl_delete(list_t *list, const char set_name) {
    // Удаление ноды с головой множества по имени set_name
    // return 0 - успешно, return -1 - не найдено

    node_t *parent = list->head;
    if (parent == NULL) return -1;
    if (parent->set->val == set_name) {
        list->size--;
        list->head = parent->next;
        free(parent);
        return 0;
    }
    while (parent->next != NULL) {
        if (parent->next->set->val == set_name) {
            list->size--;
            node_t *tmp = parent->next->next;
            destroy_set(parent->next->set);
            free(parent->next);
            parent->next = tmp;
            return 0;
        }
        parent = parent->next;
    }
    return -1;
}


void print_sets_names(const list_t *list) {
    // Вывод имен всех множеств, головы которых хранятся в списке

    putchar('[');
    if (list->head) {
        putchar(list->head->set->val);
        const node_t *cur = list->head->next;
        while (cur != NULL) {
            printf(", %c", cur->set->val);
            cur = cur->next;
        }
    }
    putchar(']');
}



#endif //FORWARD_LIST_H