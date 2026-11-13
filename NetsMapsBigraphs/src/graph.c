//
// Created by gergy on 25.10.2026.
//
#include <stdio.h>
#include <stdlib.h>

#include "graph.h"


graph_t *allocate_graph(const bool direct) {
    graph_t *graph = malloc(sizeof(graph_t));
    graph->adj_list = create_list();
    graph->direct = direct;
    return graph;
}

graph_t *read_graph(const char *filename) {
    FILE *file = fopen(filename, "r");

    if (file == nullptr) {
        printf("Error opening file %s\n", filename);
        return nullptr;
    }

    int capacity = 0;
    if (fscanf(file, "%d", &capacity) != 1 || capacity <= 0) {
        printf("Invalid graph size\n");
        fclose(file);
        return nullptr;
    }

    graph_t *graph = allocate_graph(true);
    if (graph == nullptr) {
        fclose(file);
        return nullptr;
    }
    for (int i = 0; i < capacity; ++i) {
        add_vertex(graph, i);
    }

    bool good = true;
    for (int r = 0; r < capacity; ++r) {
        for (int c = 0; c < capacity; ++c) {
            int weight = 0;
            if (fscanf(file, "%d", &weight) != 1) {
                printf("Error reading matrix at row %d col %d\n", r, c);
                good = false;
                break;
            }
            if (weight != 0) {
                add_edge(graph, r, c, weight);
            }
        }
        if (!good) break;
    }

    if (!good) {
        fclose(file);
        free_graph(graph);
        return nullptr;
    }

    fclose(file);
    return graph;
}


bool is_vertex(const graph_t *graph, const int u) {
    return find_set(graph->adj_list, u) != nullptr;
}


bool add_edge(graph_t *graph, const int u, const int v, const int w) {
    if (!is_vertex(graph, u) || !is_vertex(graph, v) || is_edge(graph, u, v)) {
        return false;
    }
    set_t *u_set = find_set(graph->adj_list, u);
    insert(u_set, v, w);

    if (!graph->direct) {
        set_t *v_set = find_set(graph->adj_list, v);
        insert(v_set, u, w);
    }

    return true;
}

bool remove_edge(graph_t *graph, const int u, const int v) {
    if (!is_vertex(graph, u) || !is_vertex(graph, v) || !is_edge(graph, u, v)) {
        return false;
    }
    set_t *u_set = find_set(graph->adj_list, u);
    delete(u_set, v);

    if (!graph->direct) {
        set_t *v_set = find_set(graph->adj_list, v);
        delete(v_set, u);
    }

    return true;
}


size_t graph_size(const graph_t *graph) {
    return graph->adj_list->size;
}


void free_graph(graph_t *graph) {
    if (graph == nullptr) return;
    destroy_list(graph->adj_list);
    free(graph);
}


int edge_weight(const graph_t *graph, const int u, const int v) {
    if (!is_edge(graph, u, v)) {
        return -1;
    }
    const set_t *u_set = find_set(graph->adj_list, u);
    const edge_t *e = find_edge(u_set, v);
    if (e == nullptr) {
        return -1;
    }
    return e->weight;
}


bool is_edge(const graph_t *graph, const int u, const int v) {
    if (!is_vertex(graph, u) || !is_vertex(graph, v)) {
        return false;
    }
    const set_t *u_set = find_set(graph->adj_list, u);
    const edge_t *e = find_edge(u_set, v);

    return e != nullptr;
}

bool add_vertex(graph_t *graph, const int u) {
    if (is_vertex(graph, u)) {
        return false;
    }
    add_set(graph->adj_list, u);
    return true;
}


bool remove_vertex(graph_t *graph, const int u) {
    if (!is_vertex(graph, u)) {
        return false;
    }
    delete_set(graph->adj_list, u);

    const node_t *cur_node = graph->adj_list->head;
    while (cur_node != nullptr) {
        set_t *cur_set = cur_node->set;
        delete(cur_set, u);
        cur_node = cur_node->next;
    }
    return true;
}

edge_t *edge_array(const graph_t *graph, size_t *capacity) {
    size_t size = 0;
    const node_t *cur_node = graph->adj_list->head;
    while (cur_node != nullptr) {
        size += cur_node->set->edge.weight;
        cur_node = cur_node->next;
    }

    if (size == 0) {
        *capacity = 0;
        return nullptr;
    }
    edge_t *edges = malloc(size * sizeof(edge_t));
    cur_node = graph->adj_list->head;
    size_t i = 0;
    while (cur_node != nullptr) {
        const set_t *cur_set = cur_node->set->next;
        while (cur_set != nullptr) {
            edges[i] = cur_set->edge;
            cur_set = cur_set->next;
            ++i;
        }
        cur_node = cur_node->next;
    }
    *capacity = i;

    return edges;
}

edge_t *edge_array_adj(const graph_t *graph, const int u, size_t *capacity) {
    if (!is_vertex(graph, u)) {
        return nullptr;
    }

    const set_t *u_set = find_set(graph->adj_list, u);
    const size_t size = u_set->edge.weight;
    size_t i = 0;
    edge_t *edges = malloc(size * sizeof(edge_t));

    u_set = u_set->next;
    while (u_set != nullptr) {
        edges[i] = u_set->edge;
        u_set = u_set->next;
        ++i;
    }
    *capacity = i;

    return edges;
}

int edge_comp(const void *a, const void *b) {
    const edge_t x = *(const edge_t *) a;
    const edge_t y = *(const edge_t *) b;

    if (x.weight < y.weight) {
        return -1;
    }
    if (x.weight > y.weight) {
        return 1;
    }
    return 0;
}
