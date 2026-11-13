#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "graph.h"
#include "net.h"
#include "map.h"
#include "bipart.h"

typedef enum Mode {
    NETWORK,
    MAP,
    BIPARTITE
} mode_t;

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Need at least 2 arguments!\n");
        return -1;
    }
    mode_t mode;
    if (strcmp(argv[1], "net") == 0) mode = NETWORK;
    else if (strcmp(argv[1], "map") == 0) mode = MAP;
    else if (strcmp(argv[1], "bipart") == 0) mode = BIPARTITE;
    else {
        printf("Unrecognized mode: %s!\n", argv[1]);
        return -1;
    }

    const char *fin = nullptr;
    const char *fout = "ans.txt";
    point_t start = {-1, -1};
    point_t end = {-1, -1};

    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "-o") == 0) {
            if (i + 1 < argc) {
                fout = argv[++i];
            } else {
                printf("Option -o requires a filename!\n");
                return -1;
            }
        } else if (strcmp(argv[i], "-a") == 0) {
            if (i + 2 < argc) {
                char *p_end = nullptr;
                start.r = (int)strtol(argv[++i], &p_end, 10);
                if (p_end == argv[i]) {
                    printf("Invalid start coordinate row: %s\n", argv[i]);
                    return -1;
                }
                start.c = (int)strtol(argv[++i], &p_end, 10);
                if (p_end == argv[i]) {
                    printf("Invalid start coordinate col: %s\n", argv[i]);
                    return -1;
                }
            } else {
                printf("Option -a requires two coordinates (row and col)!\n");
                return -1;
            }
        } else if (strcmp(argv[i], "-b") == 0) {
            if (i + 2 < argc) {
                char *p_end = nullptr;
                end.r = (int)strtol(argv[++i], &p_end, 10);
                if (p_end == argv[i]) {
                    printf("Invalid end coordinate row: %s\n", argv[i]);
                    return -1;
                }
                end.c = (int)strtol(argv[++i], &p_end, 10);
                if (p_end == argv[i]) {
                    printf("Invalid end coordinate col: %s\n", argv[i]);
                    return -1;
                }
            } else {
                printf("Option -b requires two coordinates (row and col)!\n");
                return -1;
            }
        } else if (argv[i][0] == '-') {
            printf("Unrecognized option: %s!\n", argv[i]);
            return -1;
        } else {
            if (fin == nullptr) {
                fin = argv[i];
            } else {
                printf("Unexpected positional argument: %s!\n", argv[i]);
                return -1;
            }
        }
    }

    if (fin == nullptr) {
        printf("No input file specified!\n");
        return -1;
    }

    if (mode == MAP) {
        if (start.r == -1 || start.c == -1 || end.r == -1 || end.c == -1) {
            printf("-a or -b is not specified or incomplete for map mode!\n");
            return -1;
        }
        run_map(fin, start, end, fout);
        return 0;
    }

    graph_t *graph = read_graph(fin);
    if (graph == nullptr) {
        return -1;
    }

    if (mode == NETWORK) {
        run_network(graph, fout);
    } else {
        run_bipartite(graph, fout);
    }

    free_graph(graph);
    return 0;
}
