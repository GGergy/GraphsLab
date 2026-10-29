// Реализация функционала множеств
#ifndef SET_H
#define SET_H

#pragma once


typedef struct Edge {
    int from;
    int to;
    int weight;
} edge_t;


typedef struct set {
    edge_t edge;
    struct set *next;
} set_t;


set_t *create_set(int name);
void destroy_set(set_t *set);
bool insert(set_t *set, int to, int weight);
edge_t *find_edge(const set_t *set, int to);
bool delete(set_t *set, int to);
size_t set_size(const set_t *set);
void print_set(const set_t *set);

#endif
