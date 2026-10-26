//
// Created by gergy on 25.10.2026.
//
#pragma once
#include "set.h"
#include "forward_list.h"

#ifndef GRAPHSLAB_GRAPH_H
#define GRAPHSLAB_GRAPH_H


typedef struct Graph{
    char oriented;
    list_t adj_list;
} graph_t;

typedef int pair_t[2];


graph_t *init_graph(char *filename);
void free_graph(graph_t *graph);
size_t graph_size(graph_t *graph);
int edge_weight(graph_t *graph, int u, int v);
char is_edge(graph_t *graph, int u, int v);
int add_vertex(graph_t *graph, int u);
int add_edge(graph_t *graph, int u, int v, int w);
pair_t** list_of_edges(graph_t *graph);
pair_t** list_of_edges_adg(graph_t* graph, int u);

#endif //GRAPHSLAB_GRAPH_H