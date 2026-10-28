//
// Created by gergy on 25.10.2026.
//
#ifndef GRAPHSLAB_GRAPH_H
#define GRAPHSLAB_GRAPH_H

#pragma once
#include "forward_list.h"
#include "set.h"

typedef struct Graph{
    bool direct;
    list_t *adj_list;
} graph_t;


graph_t *init_graph(char *filename);
void free_graph(graph_t *graph);
size_t graph_size(const graph_t *graph);
int edge_weight(const graph_t *graph, int u, int v);
bool is_edge(const graph_t *graph, int u, int v);
bool is_vertex(const graph_t *graph, int u);
bool add_vertex(const graph_t *graph, int u);
bool add_edge(const graph_t *graph, int u, int v, int w);
set_t* list_of_edges(const graph_t *graph);
set_t* list_of_edges_adj(const graph_t* graph, int u);
edge_t *edge_array(const graph_t *graph, size_t *capacity);
edge_t *edge_array_adj(const graph_t *graph, int u, size_t *capacity);
int *dijkstra(const graph_t *graph, int u, size_t *capacity);
graph_t *kruskal(const graph_t *graph);


#endif //GRAPHSLAB_GRAPH_H