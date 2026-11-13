#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "graph.h"
#include "net.h"

// Определение источника (in_deg == 0, out_deg > 0) и стока (out_deg == 0, in_deg > 0)
static bool find_source_and_sink(const graph_t *graph, int *out_source, int *out_sink) {
    const size_t n = graph_size(graph);
    if (n < 2) {
        printf("Network must have at least 2 vertices!\n");
        return false;
    }
    // ~Полустепени исхода и захода каждой вершины
    int *in_degree = calloc(n, sizeof(int));
    int *out_degree = calloc(n, sizeof(int));

    const node_t *cur_node = graph->adj_list->head;
    while (cur_node != nullptr) {
        const int u = cur_node->set->edge.from;
        const set_t *cur_edge = cur_node->set->next;
        while (cur_edge != nullptr) {
            const int v = cur_edge->edge.to;
            if (u >= 0 && (size_t)u < n && v >= 0 && (size_t)v < n) {
                out_degree[u]++;
                in_degree[v]++;
            }
            cur_edge = cur_edge->next;
        }
        cur_node = cur_node->next;
    }

    int source = -1;
    int sink = -1;
    int source_count = 0;
    int sink_count = 0;

    for (size_t i = 0; i < n; ++i) {
        if (in_degree[i] == 0 && out_degree[i] > 0) {
            source = (int)i;
            source_count++;
        }
        if (out_degree[i] == 0 && in_degree[i] > 0) {
            sink = (int)i;
            sink_count++;
        }
    }

    free(in_degree);
    free(out_degree);

    if (source_count != 1 || sink_count != 1) {
        printf("Error: Network must have exactly 1 source and 1 sink. Found: %d source(s), %d sink(s).\n",
               source_count, sink_count);
        return false;
    }

    *out_source = source;
    *out_sink = sink;
    return true;
}

// Построение остаточного графа на основе graph_t:
// - для каждого исходного ребра u -> v с весом cap добавляем прямое ребро (cap) и обратное ребро (0)
static graph_t *build_residual_graph(const graph_t *graph, const size_t n) {
    graph_t *res_graph = allocate_graph(true);
    for (size_t i = 0; i < n; ++i) {
        add_vertex(res_graph, (int)i);
    }

    const node_t *cur_node = graph->adj_list->head;
    while (cur_node != nullptr) {
        const int u = cur_node->set->edge.from;
        const set_t *cur_edge = cur_node->set->next;
        while (cur_edge != nullptr) {
            const int v = cur_edge->edge.to;
            const int cap = cur_edge->edge.weight;

            if (!is_edge(res_graph, u, v)) {
                // ~Прямое ребро с весом, равным пропускной способности
                add_edge(res_graph, u, v, cap);
            }
            // ~ Обратное ребро с весом 0
            if (!is_edge(res_graph, v, u)) {
                add_edge(res_graph, v, u, 0);
            }

            cur_edge = cur_edge->next;
        }
        cur_node = cur_node->next;
    }

    return res_graph;
}

// Поиск увеличивающего пути в остаточной сети через BFS (алгоритм Эдмондса-Карпа)
static bool bfs_augmenting_path(const graph_t *residual, const int s, const int t, const size_t n, int *parent) {
    for (size_t i = 0; i < n; ++i) {
        parent[i] = -1; // ~Массив для восстановления пути
    }

    int *queue = malloc(n * sizeof(int)); // ~ Очередь для BFS
    size_t q_head = 0;
    size_t q_tail = 0;

    queue[q_tail++] = s; // ~Помещаем источник в начало очереди
    parent[s] = s;

    while (q_head < q_tail) {
        const int u = queue[q_head++]; // ~Текущая вершина - из начала очереди
        if (u == t) break; // ~Если пришли в сток - конец

        const set_t *u_set = find_set(residual->adj_list, u);
        if (u_set == nullptr) continue;
        // ~Обход всех соседей текущей вершины
        const set_t *cur_edge = u_set->next;
        while (cur_edge != nullptr) {
            const int v = cur_edge->edge.to;
            const int residual_cap = cur_edge->edge.weight;
            // ~Если вершина непосещенная и имеется пропускная способность, идем туда
            if (residual_cap > 0 && parent[v] == -1) {
                parent[v] = u;
                queue[q_tail++] = v;
            }
            cur_edge = cur_edge->next;
        }
    }

    const bool found = (parent[t] != -1); // ~цепь найдена, если пришли в сток
    free(queue);
    return found;
}

// Алгоритм Эдмондса-Карпа
static int edmonds_karp(const graph_t *residual, const int s, const int t, const size_t n) {
    int *parent = malloc(n * sizeof(int));
    int max_flow = 0;

    // ~Поток можно улучшить, пока есть дополняющая цепь из источника в сток
    while (bfs_augmenting_path(residual, s, t, n, parent)) {
        // Вычисляем bottleneck (минимальную остаточную пропускную способность вдоль пути)
        int bottleneck = INT_MAX;
        // ~Восстанавливаем путь из массива parent
        for (int v = t; v != s; v = parent[v]) {
            const int u = parent[v];
            const int cap = edge_weight(residual, u, v);
            if (cap < bottleneck) {
                bottleneck = cap; // ~Наименьшая пропускная способность дополняющей цепи
            }
        }

        // Модифицируем остаточные пропускные способности
        for (int v = t; v != s; v = parent[v]) {
            const int u = parent[v];

            // ~Для ребер из источника в сток остаточная пропускная способность уменьшается
            const set_t *u_set = find_set(residual->adj_list, u);
            edge_t *fwd_edge = find_edge(u_set, v);
            fwd_edge->weight -= bottleneck;

            // ~Для ребер из стока в источник остаточная пропускная способность увеличивается
            const set_t *v_set = find_set(residual->adj_list, v);
            edge_t *bwd_edge = find_edge(v_set, u);
            bwd_edge->weight += bottleneck;
        }

        max_flow += bottleneck; // Поток в сети увеличивается также на bottleneck
    }

    free(parent);
    return max_flow;
}

// Запись результатов в выходной файл согласно ТЗ
static bool write_network_result(const char *fout, const int s, const int t,
                                 const int max_flow, const graph_t *orig, const graph_t *residual) {
    FILE *file = fopen(fout, "w");
    if (file == nullptr) {
        printf("Error: Could not open output file %s\n", fout);
        return false;
    }

    fprintf(file, "Source: %d\n", s);
    fprintf(file, "Sink: %d\n", t);
    fprintf(file, "Max flow: %d\n", max_flow);
    fprintf(file, "Edges (u -> v: flow / capacity):\n");

    const node_t *cur_node = orig->adj_list->head;
    while (cur_node != nullptr) {
        const int u = cur_node->set->edge.from;
        const set_t *cur_edge = cur_node->set->next;
        while (cur_edge != nullptr) {
            const int v = cur_edge->edge.to;
            const int original_cap = cur_edge->edge.weight;
            const int remaining_cap = edge_weight(residual, u, v);
            const int flow = original_cap - remaining_cap;

            fprintf(file, "%d -> %d: %d / %d\n", u, v, flow, original_cap);
            cur_edge = cur_edge->next;
        }
        cur_node = cur_node->next;
    }

    fclose(file);
    return true;
}

void run_network(const graph_t *graph, const char *fout) {
    if (graph == nullptr) {
        printf("Error: Graph is null!\n");
        return;
    }

    int source = -1;
    int sink = -1;
    if (!find_source_and_sink(graph, &source, &sink)) {
        return;
    }

    const size_t n = graph_size(graph);
    graph_t *residual = build_residual_graph(graph, n); // ~Остаточная сеть

    const int max_flow = edmonds_karp(residual, source, sink, n);

    if (write_network_result(fout, source, sink, max_flow, graph, residual)) {
        printf("Network flow finished successfully. Result saved to %s\n", fout);
    }

    free_graph(residual);
}
