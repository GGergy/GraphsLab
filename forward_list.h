// Реализация простого односвязного списка для хранения голов множеств
#ifndef FORWARD_LIST_H
#define FORWARD_LIST_H

#pragma once
#include "set.h"


typedef struct Node {
    set_t *set;
    struct Node *next;
} node_t;


typedef struct List {
    node_t *head;
    size_t size;
} list_t;


list_t *create_list();
void destroy_list(list_t *list);
set_t *add_set(list_t* list, int set_name);
set_t *find_set(const list_t *list, int set_name);
bool delete_set(list_t *list, int set_name);
void print_sets_names(const list_t *list);

#endif //FORWARD_LIST_H
