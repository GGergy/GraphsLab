#include <stdio.h>
#include <stdlib.h>

#include "graph.h"

int main(void) {
    const graph_t *graph = init_graph("../graph.txt");
    if (graph == nullptr) {
        printf("Error allocating graph\n");
        return -1;
    }
    size_t capacity;
    edge_t *edges_arr = edge_array_adj(graph, 1, &capacity);
    printf("Edges: %lld\n", capacity);
    free(edges_arr);
}
