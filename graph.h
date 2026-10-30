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


typedef struct Dist {
    int vertex;
    size_t distance;
} dist_t;

typedef struct Path {
    int *path;
    size_t length;
    size_t total_dist;
} path_t;


graph_t *allocate_graph(bool direct);
graph_t *read_graph(char *filename);
void free_graph(graph_t *graph);
size_t graph_size(const graph_t *graph);
int edge_weight(const graph_t *graph, int u, int v);
bool is_edge(const graph_t *graph, int u, int v);
bool is_vertex(const graph_t *graph, int u);
bool add_vertex(const graph_t *graph, int u);
bool remove_vertex(const graph_t *graph, int u);
bool add_edge(const graph_t *graph, int u, int v, int w);
bool remove_edge(const graph_t *graph, int u, int v);
edge_t *edge_array(const graph_t *graph, size_t *capacity);
edge_t *edge_array_adj(const graph_t *graph, int u, size_t *capacity);
dist_t *dijkstra(const graph_t *graph, int u, size_t *capacity);
path_t *dijkstra_path(const graph_t *graph, int u, int v);
graph_t *kruskal(const graph_t *graph);
void print_graph_conns(const graph_t *graph);
void tarjan(const graph_t *graph);


#endif //GRAPHSLAB_GRAPH_H
