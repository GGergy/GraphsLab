//
// Created by gergy on 25.10.2026.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "graph.h"


graph_t *init_graph(char *filename) {
    FILE *file = fopen(filename, "r");

    if (file == nullptr) {
        printf("Error opening file %s\n", filename);
        return nullptr;
    }

    graph_t *graph = malloc(sizeof(graph_t));
    char buf[100];

    fgets(buf, 100, file);
    buf[strcspn(buf, "\r\n")] = '\0';
    if (strcmp(buf, "digraph") == 0) {
        graph->direct = true;
        printf("Detected type Digraph\n");
    } else if (strcmp(buf, "graph") == 0) {
        graph->direct = false;
        printf("Detected type Graph\n");
    } else {
        printf("Invalid graph type %s\n", buf);
        return nullptr;
    }
    graph->adj_list = create_list();
    size_t line_num = 1;

    while (fgets(buf, 100, file) != nullptr) {
        buf[strcspn(buf, "\r\n")] = '\0';
        constexpr char delim[] = " \t";
        char bufcpy[100];
        strcpy(bufcpy, buf);
        const char *token = strtok(bufcpy, delim);
        int from = 0;
        int to = 0;
        int weight = 0;
        int narg = 0;

        while (token != nullptr) {
            if (strlen(token) == 0) continue;
            char *end;
            switch (narg) {
                case 0:
                    from = strtol(token, &end, 10);
                    break;
                case 1:
                    to = strtol(token, &end, 10);
                    break;
                case 2:
                    weight = strtol(token, &end, 10);
                    break;
                default:
                    printf("Invalid line #%lld: '%s'\n", line_num, buf);
                    return nullptr;
            }
            if (end == token) {
                printf("Invalid digit on line #%lld: '%s'\n", line_num, token);
                return nullptr;
            }
            ++narg;
            token = strtok(nullptr, delim);
        }
        if (narg == 3) {
            if (is_edge(graph, from, to)) {
                printf("Duplicate edge on line #%lld: [%d->%d, w:%d]\n", line_num, from, to, weight);
                return nullptr;
            }
            if (from == to) {
                printf("Loop edge on line #%lld: [%d->%d, w:%d]\n", line_num, from, to, weight);
                return nullptr;
            }
            if (weight <= 0) {
                printf("Non-positive weight on line #%lld: [%d->%d, w:%d]\n", line_num, from, to, weight);
                return nullptr;
            }

            add_vertex(graph, from);
            add_vertex(graph, to);
            add_edge(graph, from, to, weight);
            printf("add edge %d->%d\n", from, to);
            if (!graph->direct) {
                printf("add reverse edge %d->%d\n", to, from);
                add_edge(graph, to, from, weight);
            }
            ++line_num;
        } else {
            printf("Invalid line #%lld: '%s'\n", line_num, buf);
            return nullptr;
        }
    }

    fclose(file);
    return graph;
}


bool is_vertex(const graph_t *graph, const int u) {
    return find_set(graph->adj_list, u) != nullptr;
}


bool add_edge(const graph_t *graph, const int u, const int v, const int w) {
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


size_t graph_size(const graph_t *graph) {
    return graph->adj_list->size;
}


void free_graph(graph_t *graph) {
    destroy_list(graph->adj_list);
    free(graph);
}


int edge_weight(const graph_t *graph, const int u, const int v) {
    if (!is_edge(graph, u, v)) {
        return -1;
    }
    const set_t *u_set = find_set(graph->adj_list, u);
    const edge_t *e = find_edge(u_set, u);
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

bool add_vertex(const graph_t *graph, const int u) {
    if (is_vertex(graph, u)) {
        return false;
    }
    add_set(graph->adj_list, u);
    return true;
}

edge_t *edge_array(const graph_t *graph, size_t *capacity) {
    size_t size = 0;
    const node_t *cur_node = graph->adj_list->head;
    while (cur_node != nullptr) {
        size += cur_node->set->edge.weight;
        cur_node = cur_node->next;
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


size_t *dijkstra(const graph_t *graph, const int u, size_t *capacity) {
    const size_t size = graph_size(graph);
    size_t *dist = malloc(size * sizeof(size_t));

    for (int i = 0; i < size; ++i) {
        if (i == u) {
            dist[i] = 0;
        }
        else {
            dist[i] = ULLONG_MAX;
        }
    }

    return dist;
}