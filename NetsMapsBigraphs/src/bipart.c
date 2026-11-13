#include <stdio.h>
#include <stdlib.h>

#include "graph.h"
#include "bipart.h"


typedef struct EdgePair {
    int u;
    int v;
} edge_pair_t;


// ~Приведение графа к неориентированному виду - добавление обратных ребер
static void make_graph_undirected(graph_t *graph) {
    if (!graph->direct) return;

    size_t edge_count = 0;
    edge_t *edges = edge_array(graph, &edge_count);
    if (edges == nullptr) return;

    for (size_t i = 0; i < edge_count; ++i) {
        const int u = edges[i].from;
        const int v = edges[i].to;
        if (!is_edge(graph, v, u)) {
            add_edge(graph, v, u, edges[i].weight);
        }
    }
    graph->direct = false;
    free(edges);
}

// Проверка графа на двудольность через BFS и раскрашивание вершин
static bool check_bipartite(const graph_t *graph, const size_t n, int *colors) {
    for (size_t i = 0; i < n; ++i) {
        colors[i] = 0; // ~0 - вершина не раскрашена, 1 и 2 - первая и вторая доля
    }

    int *queue = malloc(n * sizeof(int));

    // ~Обход BFS на стеке
    for (size_t start = 0; start < n; ++start) {
        if (colors[start] != 0) continue;

        size_t q_head = 0;
        size_t q_tail = 0;

        colors[start] = 1; // ~Стартовая вершина в первой доле
        queue[q_tail++] = (int)start;

        while (q_head < q_tail) {
            const int u = queue[q_head++];
            const set_t *u_set = find_set(graph->adj_list, u);
            if (u_set == nullptr) continue;

            const set_t *cur_edge = u_set->next;
            // ~Смотрим всех соседей вершины с начала очереди
            while (cur_edge != nullptr) {
                const int to = cur_edge->edge.to;
                if (colors[to] == 0) {
                    // ~Если сосед не раскрашен, красим в другой цвет
                    colors[to] = (colors[u] == 1) ? 2 : 1;
                    queue[q_tail++] = to; // ~Помещаем соседа в конец очереди
                } else if (colors[to] == colors[u]) {
                    // ~Если ребро соединяет вершины из одной доли, граф не двудолен
                    free(queue);
                    return false;
                }
                cur_edge = cur_edge->next;
            }
        }
    }

    free(queue);
    return true;
}

// Алгоритм Куна: поиск дополняющей цепи через прямой обход списка смежности вершины u
static bool try_kuhn(const graph_t *graph, const int u, bool *visited, int *match) {
    if (visited[u]) return false;
    visited[u] = true;

    const set_t *u_set = find_set(graph->adj_list, u);
    if (u_set == nullptr) return false;

    const set_t *cur_edge = u_set->next;
    // ~Перебираем всех соседей u
    while (cur_edge != nullptr) {
        const int to = cur_edge->edge.to;
        if (match[to] == -1 || try_kuhn(graph, match[to], visited, match)) {
            // ~Если вершина свободна, или того, кто ее занимает можно связать с другой свободной вершиной,
            // ~то цепь найдена
            match[to] = u;
            return true;
        }
        cur_edge = cur_edge->next;
    }
    return false;
}

// Поиск максимального паросочетания с использованием структуры graph_t
static size_t find_max_matching(const graph_t *graph, const size_t n, const int *colors, edge_pair_t **out_edges) {
    int *match = malloc(n * sizeof(int)); // ~Индекс вершины из противоположной доли, с которой спарена вершина
    bool *visited = malloc(n * sizeof(bool)); // ~Метки для алгоритма Куна

    for (size_t i = 0; i < n; ++i) {
        match[i] = -1;
    }

    size_t matching_size = 0;
    // Запускаем поиск дополняющих цепей из вершин первой доли (color == 1)
    for (size_t u = 0; u < n; ++u) {
        if (colors[u] == 1) {
            // ~Сбрасываем метки
            for (size_t i = 0; i < n; ++i) {
                visited[i] = false;
            }
            // ~Поиск дополняющей цепи
            if (try_kuhn(graph, (int)u, visited, match)) {
                // ~Размер паросочетания равен количеству найденных дополняющих цепей
                matching_size++;
            }
        }
    }

    // Сохраняем рёбра найденного паросочетания (1-based индексы для отчёта)
    edge_pair_t *edges = malloc(matching_size * sizeof(edge_pair_t));
    size_t idx = 0;
    for (size_t v = 0; v < n; ++v) {
        if (match[v] != -1 && colors[v] == 2) {
            edges[idx].u = match[v] + 1;
            edges[idx].v = (int)v + 1;
            idx++;
        }
    }

    free(match);
    free(visited);
    *out_edges = edges;
    return matching_size;
}

void run_bipartite(graph_t *graph, const char *fout) {
    if (graph == nullptr) {
        printf("Error: Graph is null!\n");
        return;
    }

    const size_t n = graph_size(graph);
    if (n == 0) {
        printf("Graph is empty!\n");
        return;
    }

    // Приводим граф к неориентированному виду
    make_graph_undirected(graph);

    // ~Цвета вершин - отношение к 1 или 2 доле
    int *colors = malloc(n * sizeof(int));
    const bool is_bipartite = check_bipartite(graph, n, colors);

    FILE *file = fopen(fout, "w");
    if (file == nullptr) {
        printf("Error: Could not open output file %s\n", fout);
        free(colors);
        return;
    }

    if (!is_bipartite) {
        fprintf(file, "Is bipartite: No.\n");
        printf("Graph is not bipartite. Result saved to %s\n", fout);
    } else {
        edge_pair_t *matching_edges = nullptr;
        const size_t match_size = find_max_matching(graph, n, colors, &matching_edges);

        fprintf(file, "Is bipartite: Yes.\n");
        fprintf(file, "Number of matchings: %zu.\n", match_size);
        fprintf(file, "Size of maximum matching: %zu.\n", match_size);
        fprintf(file, "Maximum matching:\n{");
        for (size_t i = 0; i < match_size; ++i) {
            fprintf(file, "(%d, %d)", matching_edges[i].u, matching_edges[i].v);
            if (i + 1 < match_size) {
                fprintf(file, ", ");
            }
        }
        fprintf(file, "}\n");

        free(matching_edges);
        printf("Bipartite matching finished successfully. Result saved to %s\n", fout);
    }

    fclose(file);
    free(colors);
}
