#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "graph.h"

int main(void) {
    graph_t *graph = nullptr;
    char buf[100];

    while (fgets(buf, 100, stdin) != nullptr) {
        buf[strcspn(buf, "\r\n")] = '\0';
        constexpr char delim[] = " \t";
        char bufcpy[100];
        strcpy(bufcpy, buf);
        const char *token = strtok(bufcpy, delim);
        char tok1[100];
        char tok2[100];
        char tok3[100];
        char tok4[100];
        int narg = 0;

        while (token != nullptr) {
            if (strlen(token) == 0) continue;
            switch (narg) {
                case 0:
                    strcpy(tok1, token);
                    break;
                case 1:
                    strcpy(tok2, token);
                    break;
                case 2:
                    strcpy(tok3, token);
                    break;
                case 3:
                    strcpy(tok4, token);
                    break;
                default:
                    printf("Too many arguments\n");
                    break;
            }
            ++narg;
            if (narg > 4) break;
            token = strtok(nullptr, delim);
        }

        if (narg == 0 || narg > 4) continue;
        if (strcmp(tok1, "create") == 0 && narg == 2) {
            if (strcmp(tok2, "graph") == 0) {
                if (graph != nullptr) free_graph(graph);

                graph = allocate_graph(false);
                printf("Created graph\n");
            } else if (strcmp(tok2, "digraph") == 0) {
                if (graph != nullptr) free_graph(graph);

                graph = allocate_graph(true);
                printf("Created digraph\n");
            } else {
                printf("Invalid graph type\n");
            }
        } else if (strcmp(tok1, "read") == 0 && narg == 2) {
            if (graph != nullptr) free_graph(graph);

            graph = read_graph(tok2);
            if (graph != nullptr) {
                printf("Successfully read graph from %s\n", tok2);
            }
        } else if (strcmp(tok1, "add_v") == 0 && narg == 2) {
            char *end = nullptr;
            const int vertex = strtol(tok2, &end, 10);
            if (end == tok2) {
                printf("Invalid vertex num\n");
            } else if (graph == nullptr) {
                printf("Graph not created!\n");
            } else {
                if (!add_vertex(graph, vertex)) {
                    printf("Vertex already exists!\n");
                } else {
                    printf("Add vertex %d\n", vertex);
                }
            }
        } else if (strcmp(tok1, "add_e") == 0 && narg == 4) {
            char *e1, *e2, *e3;
            const int from = strtol(tok2, &e1, 10);
            const int to = strtol(tok3, &e2, 10);
            const int w = strtol(tok4, &e3, 10);
            if (e1 == tok2 || e2 == tok3 || e3 == tok4) {
                printf("Invalid digit\n");
            } else if (graph == nullptr) {
                printf("Graph not created!\n");
            } else {
                if (!add_edge(graph, from, to, w)) {
                    printf("Edge already exists or cannot exist!\n");
                } else {
                    printf("Add edge %d->%d[%d]\n", from, to, w);
                }
            }
        } else if (strcmp(tok1, "rem_v") == 0 && narg == 2) {
            char *end = nullptr;
            const int vertex = strtol(tok2, &end, 10);
            if (end == tok2) {
                printf("Invalid vertex num\n");
            } else if (graph == nullptr) {
                printf("Graph not created!\n");
            } else {
                if (!remove_vertex(graph, vertex)) {
                    printf("Vertex not exists!\n");
                } else {
                    printf("Remove vertex %d\n", vertex);
                }
            }
        } else if (strcmp(tok1, "rem_e") == 0 && narg == 3) {
            char *e1, *e2;
            const int from = strtol(tok2, &e1, 10);
            const int to = strtol(tok3, &e2, 10);
            if (e1 == tok2 || e2 == tok3) {
                printf("Invalid digit\n");
            } else if (graph == nullptr) {
                printf("Graph not created!\n");
            } else {
                if (!remove_edge(graph, from, to)) {
                    printf("Edge not exists!\n");
                } else {
                    printf("Remove edge %d->%d\n", from, to);
                }
            }
        } else if (strcmp(tok1, "conn") == 0 && narg == 1) {
            if (graph == nullptr) {
                printf("Graph not created!\n");
            } else {
                if (graph->direct) {
                    printf("Week connectivity:\n");
                } else {
                    printf("Connectivity:\n");
                }
                print_graph_conns(graph);
                if (graph->direct) {
                    printf("Strong connectivity:\n");
                    print_strong_conn(graph);
                }
            }
        } else if (strcmp(tok1, "tree") == 0 && narg == 1) {
            if (graph == nullptr) {
                printf("Graph not created!\n");
            } else {
                graph_t *tree = kruskal(graph);
                size_t capacity = 0;
                edge_t *edges = edge_array(tree, &capacity);
                printf("Tree: ");
                int weight = 0;
                for (size_t i = 0; i < capacity; i++) {
                    if (!graph->direct && edges[i].from > edges[i].to) continue;
                    printf("%d->%d[%d] ", edges[i].from, edges[i].to, edges[i].weight);
                    weight += edges[i].weight;
                }
                printf("Total: [%d]\n", weight);
                free(edges);
                free(tree);
            }
        } else if (strcmp(tok1, "vertex") == 0 && narg == 1) {
            if (graph == nullptr) {
                printf("Graph not created!\n");
            }
            print_sets_names(graph->adj_list);
            putchar('\n');
        } else if (strcmp(tok1, "edges") == 0) {
            size_t capacity = 0;
            if (graph == nullptr) {
                printf("Graph not created!\n");
            } else if (narg == 1) {
                edge_t *edges = edge_array(graph, &capacity);
                printf("All edges: ");
                if (capacity == 0) {
                    printf("No edges");
                } else {
                    for (size_t i = 0; i < capacity; i++) {
                        if (!graph->direct && edges[i].from > edges[i].to) continue;
                        printf("%d->%d[%d] ", edges[i].from, edges[i].to, edges[i].weight);
                    }
                }
                printf("\n");
                free(edges);
            } else if (narg == 2) {
                char *end = nullptr;
                const int vertex = strtol(tok2, &end, 10);
                if (end == tok2) {
                    printf("Invalid vertex num\n");
                }
                edge_t *edges = edge_array_adj(graph, vertex, &capacity);
                printf("Edges adjusted to <%d>: ", vertex);
                if (capacity == 0) {
                    printf("No edges");
                } else {
                    for (size_t i = 0; i < capacity; i++) {
                        printf("%d->%d[%d] ", edges[i].from, edges[i].to, edges[i].weight);
                    }
                }
                printf("\n");
                free(edges);
            } else {
                printf("Invalid input!\n");
            }
        } else if (strcmp(tok1, "path") == 0) {
            size_t capacity = 0;
            if (graph == nullptr) {
                printf("Graph not created!\n");
            } else if (narg == 2) {
                char *end = nullptr;
                const int vertex = strtol(tok2, &end, 10);
                if (end == tok2) {
                    printf("Invalid vertex num\n");
                }
                dist_t *dists = dijkstra(graph, vertex, &capacity);
                if (dists == nullptr) {
                    printf("No vertex\n");
                    continue;
                }
                printf("Distances from <%d>: ", vertex);
                if (capacity == 0) {
                    printf("No edges");
                } else {
                    for (size_t i = 0; i < capacity; i++) {
                        printf("[%lld]->%d ", dists[i].distance, dists[i].vertex);
                    }
                }
                printf("\n");
                free(dists);
            } else if (narg == 3) {
                char *e1, *e2;
                const int from = strtol(tok2, &e1, 10);
                const int to = strtol(tok3, &e2, 10);
                if (e1 == tok2 || e2 == tok3) {
                    printf("Invalid digit\n");
                } else if (graph == nullptr) {
                    printf("Graph not created!\n");
                }
                path_t *path = dijkstra_path(graph, from, to);
                if (path == nullptr) {
                    printf("Path not found\n");
                } else {
                    printf("Path from <%d> to <%d>: Length: %lld, Weight: %lld\n", from, to, path->length,
                           path->total_dist);
                    for (size_t i = 0; i < path->length - 1; i++) {
                        printf("%d->", path->path[i]);
                    }
                    printf("%d\n", to);
                    free(path->path);
                    free(path);
                }
            } else {
                printf("Invalid input!\n");
            }
        } else {
            printf("Invalid input\n");
        }
    }

    free_graph(graph);
}
