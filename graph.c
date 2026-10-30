//
// Created by gergy on 25.10.2026.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "graph.h"

inline size_t ullmin(size_t a, size_t b);


graph_t *allocate_graph(const bool direct) {
    graph_t *graph = malloc(sizeof(graph_t));
    graph->adj_list = create_list();
    graph->direct = direct;
    return graph;
}

graph_t *read_graph(char *filename) {
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
        fclose(file);
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
                    destroy_list(graph->adj_list);
                    fclose(file);
                    return nullptr;
            }
            if (end == token) {
                printf("Invalid digit on line #%lld: '%s'\n", line_num, token);
                destroy_list(graph->adj_list);
                fclose(file);
                return nullptr;
            }
            ++narg;
            token = strtok(nullptr, delim);
        }
        if (narg == 3) {
            if (is_edge(graph, from, to)) {
                printf("Duplicate edge on line #%lld: [%d->%d, w:%d]\n", line_num, from, to, weight);
                destroy_list(graph->adj_list);
                fclose(file);
                return nullptr;
            }
            if (from == to) {
                printf("Loop edge on line #%lld: [%d->%d, w:%d]\n", line_num, from, to, weight);
                destroy_list(graph->adj_list);
                fclose(file);
                return nullptr;
            }
            if (weight <= 0) {
                printf("Non-positive weight on line #%lld: [%d->%d, w:%d]\n", line_num, from, to, weight);
                destroy_list(graph->adj_list);
                fclose(file);
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
            destroy_list(graph->adj_list);
            fclose(file);
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

bool remove_edge(const graph_t *graph, const int u, const int v) {
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

bool add_vertex(const graph_t *graph, const int u) {
    if (is_vertex(graph, u)) {
        return false;
    }
    add_set(graph->adj_list, u);
    return true;
}


bool remove_vertex(const graph_t *graph, const int u) {
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


size_t ullmin(const size_t a, const size_t b) {
    return a < b ? a : b;
}


dist_t *dijkstra(const graph_t *graph, const int u, size_t *capacity) {
    const size_t size = graph_size(graph);
    if (size == 0) {
        *capacity = 0;
        return nullptr;
    }
    size_t *dist = malloc(size * sizeof(size_t));
    bool *worked = malloc(size * sizeof(bool));
    int *names = malloc(size * sizeof(int));

    for (size_t i = 0; i < size; ++i) {
        worked[i] = false;
    }

    {
        size_t k = 0;
        const node_t *cur_node = graph->adj_list->head;
        while (cur_node != nullptr) {
            const set_t *cur_set = cur_node->set;
            names[k] = cur_set->edge.from;
            cur_node = cur_node->next;
            ++k;
        }
    }

    for (size_t i = 0; i < size; ++i) {
        if (names[i] == u) {
            dist[i] = 0;
        }
        else {
            dist[i] = ULLONG_MAX;
        }
    }


    while (true) {
        size_t min_dist = ULLONG_MAX;
        size_t min_index = 0;
        for (size_t i = 0; i < size; ++i) {
            if (!worked[i] && dist[i] < min_dist) {
                min_dist = dist[i];
                min_index = i;
            }
        }
        if (min_dist == ULLONG_MAX) {
            break;
        }

        for (size_t i = 0; i < size; ++i) {
            if (!worked[i] && is_edge(graph, names[min_index], names[i])) {
                dist[i] = ullmin(dist[i], dist[min_index] + edge_weight(graph, names[min_index], names[i]));
            }
        }
        worked[min_index] = true;
    }

    dist_t *response = malloc(size * sizeof(dist_t));
    for (size_t i = 0; i < size; ++i) {
        response[i].vertex = names[i];
        response[i].distance = dist[i];
    }

    free(worked);
    free(names);
    free(dist);

    *capacity = size;
    return response;
}

path_t *dijkstra_path(const graph_t *graph, const int u, const int v) {
    if (!is_vertex(graph, u) || !is_vertex(graph, v)) {
        return nullptr;
    }

    const size_t size = graph_size(graph);
    size_t *dist = malloc(size * sizeof(size_t));
    bool *worked = calloc(size, sizeof(bool));
    int *names = malloc(size * sizeof(int));
    // Из какой вершины дошли в i-ю
    size_t *prev = malloc(size * sizeof(size_t));

    {
        size_t k = 0;
        const node_t *cur_node = graph->adj_list->head;
        while (cur_node != nullptr) {
            const set_t *cur_set = cur_node->set;
            names[k] = cur_set->edge.from;
            cur_node = cur_node->next;
            ++k;
        }
    }

    size_t target_idx = ULLONG_MAX;
    for (size_t i = 0; i < size; ++i) {
        prev[i] = ULLONG_MAX;
        if (names[i] == u) {
            dist[i] = 0;
        } else {
            dist[i] = ULLONG_MAX;
        }
        if (names[i] == v) {
            target_idx = i;
        }
    }

    while (true) {
        size_t min_dist = ULLONG_MAX;
        size_t min_index = 0;
        for (size_t i = 0; i < size; ++i) {
            if (!worked[i] && dist[i] < min_dist) {
                min_dist = dist[i];
                min_index = i;
            }
        }
        if (min_dist == ULLONG_MAX) {
            break;
        }
        if (min_index == target_idx) {
            break;
        }

        for (size_t i = 0; i < size; ++i) {
            if (!worked[i] && is_edge(graph, names[min_index], names[i])) {
                const size_t new_dist = dist[min_index] + edge_weight(graph, names[min_index], names[i]);
                if (new_dist < dist[i]) {
                    dist[i] = new_dist;
                    prev[i] = min_index;
                }
            }
        }
        worked[min_index] = true;
    }

    path_t *response = malloc(sizeof(path_t));
    response->path = nullptr;
    response->length = 0;
    response->total_dist = dist[target_idx];

    // Восстановление пути
    if (dist[target_idx] != ULLONG_MAX) {
        int *temp_path = malloc(size * sizeof(int));
        size_t path_len = 0;

        size_t at = target_idx;
        while (at != ULLONG_MAX) {
            temp_path[path_len] = names[at];
            ++path_len;
            at = prev[at];
        }

        response->path = malloc(path_len * sizeof(int));
        for (size_t i = 0; i < path_len; ++i) {
            response->path[i] = temp_path[path_len - 1 - i];
        }
        response->length = path_len;
        free(temp_path);
    }

    free(prev);
    free(worked);
    free(names);
    free(dist);
    if (response->path == nullptr) {
        free(response);
        return nullptr;
    }

    return response;
}


int edge_comp(const void *a, const void *b) {
    const edge_t x = *(const edge_t *)a;
    const edge_t y = *(const edge_t *)b;

    if (x.weight < y.weight) {
        return -1;
    }
    if (x.weight > y.weight) {
        return 1;
    }
    return 0;
}


graph_t *kruskal(const graph_t *graph) {
    size_t capacity = 0;
    graph_t *tree = malloc(sizeof (graph_t));
    tree->direct = graph->direct;
    tree->adj_list = create_list();

    const size_t size = graph_size(graph);
    if (size == 0) {
        return tree;
    }

    edge_t *edges = edge_array(graph, &capacity);
    qsort(edges, capacity, sizeof(edge_t), edge_comp);


    int *names = malloc(size * sizeof(int));

    {
        size_t k = 0;
        const node_t *cur_node = graph->adj_list->head;
        while (cur_node != nullptr) {
            const set_t *cur_set = cur_node->set;
            names[k] = cur_set->edge.from;
            cur_node = cur_node->next;
            ++k;
        }
    }

    typedef struct conn_item {
        size_t vertex;
        size_t conn;
    } conn_t;

    conn_t *conn = malloc(size * sizeof(conn_t));
    for (size_t i = 0; i < size; ++i) {
        conn[i].vertex = names[i];
        conn[i].conn = i;
    }
    size_t i = 0;

    while (i < capacity) {
        const edge_t cur_edge = edges[i];
        size_t from_c = -1;
        size_t to_c = -1;

        for (size_t j = 0; j < size; ++j) {
            if (cur_edge.from == conn[j].vertex) {
                from_c = conn[j].conn;
            }
            else if (cur_edge.to == conn[j].vertex) {
                to_c = conn[j].conn;
            }
            if (from_c != -1 && to_c != -1) break;
        }

        if (from_c != to_c) {
            add_vertex(tree, cur_edge.from);
            add_vertex(tree, cur_edge.to);
            add_edge(tree, cur_edge.from, cur_edge.to, cur_edge.weight);
            if (!graph->direct) {
                add_edge(tree, cur_edge.to, cur_edge.from, cur_edge.weight);
            }
            bool is_done = true;
            for (size_t j = 0; j < size; ++j) {
                if (conn[j].conn == to_c) {
                    conn[j].conn = from_c;
                }
                else if (conn[j].conn != from_c) {
                    is_done = false;
                }
            }
            if (is_done) break;
        }

        ++i;
    }

    free(conn);
    free(edges);
    free(names);
    return tree;
}


void print_graph_conns(const graph_t *graph) {
    size_t capacity = 0;
    const size_t size = graph_size(graph);
    if (size == 0) return;
    edge_t *edges = edge_array(graph, &capacity);
    int *names = malloc(size * sizeof(int));

    {
        size_t k = 0;
        const node_t *cur_node = graph->adj_list->head;
        while (cur_node != nullptr) {
            const set_t *cur_set = cur_node->set;
            names[k] = cur_set->edge.from;
            cur_node = cur_node->next;
            ++k;
        }
    }

    typedef struct conn_item {
        size_t vertex;
        size_t conn;
    } conn_item;

    conn_item *conn = malloc(size * sizeof(conn_item));
    for (size_t i = 0; i < size; ++i) {
        conn[i].vertex = names[i];
        conn[i].conn = i;
    }

    size_t i = 0;
    while (i < capacity) {
        const edge_t cur_edge = edges[i];
        size_t from_c = -1;
        size_t to_c = -1;

        for (size_t j = 0; j < size; ++j) {
            if (cur_edge.from == conn[j].vertex) {
                from_c = conn[j].conn;
            }
            else if (cur_edge.to == conn[j].vertex) {
                to_c = conn[j].conn;
            }
            if (from_c != -1 && to_c != -1) break;
        }

        if (from_c != to_c) {
            bool is_done = true;
            for (size_t j = 0; j < size; ++j) {
                if (conn[j].conn == to_c) {
                    conn[j].conn = from_c;
                }
                else if (conn[j].conn != from_c) {
                    is_done = false;
                }
            }
            if (is_done) break;
        }

        ++i;
    }

    size_t comp_num = 0;
    for (size_t j = 0; j < size; ++j) {
        bool header = false;
        for (size_t k = 0; k < size; ++k) {
            if (conn[k].conn == j) {
                if (!header) {
                    header = true;
                    comp_num++;
                    printf("Component #%lld: %lld", comp_num, conn[k].vertex);
                }
                else {
                    printf(", %lld", conn[k].vertex);
                }
            }
        }
        if (header) printf("\n");
    }

    free(conn);
    free(edges);
    free(names);
}


void tarjan(const graph_t *graph) {
    const size_t size = graph_size(graph);
    if (size == 0) {
        return;
    }

    int *names = malloc(size * sizeof(int));
    {
        size_t k = 0;
        const node_t *cur_node = graph->adj_list->head;
        while (cur_node != nullptr) {
            const set_t *cur_set = cur_node->set;
            names[k] = cur_set->edge.from;
            cur_node = cur_node->next;
            ++k;
        }
    }
    // Стек DFS
    size_t *stack = malloc((size + 1) * sizeof(size_t));
    bool *visited = calloc(size, sizeof(bool));
    // Порядок выхода
    size_t *exit = malloc(size * sizeof(size_t));
    size_t timer = 0;
    size_t stack_size = 0;

    // Инвертированный DFS
    for (size_t i = 0; i < size; ++i) {
        if (visited[i]) continue;
        visited[i] = true;
        stack[0] = i;
        stack_size = 1;

        while (stack_size != 0) {
            bool done = true;
            const size_t cur = stack[stack_size - 1];
            for (size_t j = 0; j < size; ++j) {
                // Ребро проверяется инвертированное
                if (!visited[j] && is_edge(graph, names[j], names[cur])) {
                    done = false;
                    visited[j] = true;
                    stack[stack_size] = j;
                    ++stack_size;
                    break;
                }
            }
            if (done) {
                --stack_size;
                exit[timer] = cur;
                ++timer;
            }
        }
    }

    // Сброс отметок
    memset(visited, false, size * sizeof(bool));

    size_t comp_num = 0;
    // Прямой DFS в обратном порядке выхода
    for (size_t i = size; i > 0; --i) {
        const size_t start = exit[i - 1];
        if (visited[start]) continue;

        visited[start] = true;
        stack[0] = start;
        stack_size = 1;
        comp_num++;
        printf("Component #%lld: %d", comp_num, names[start]);

        while (stack_size != 0) {
            bool done = true;
            const size_t cur = stack[stack_size - 1];
            for (size_t j = 0; j < size; ++j) {
                // Ребро проверяется обычное
                if (!visited[j] && is_edge(graph, names[cur], names[j])) {
                    done = false;
                    visited[j] = true;
                    stack[stack_size] = j;
                    ++stack_size;
                    printf(", %d", names[j]);
                    break;
                }
            }
            if (done) {
                --stack_size;
            }
        }
        printf("\n");
    }


    free(exit);
    free(visited);
    free(stack);
    free(names);
}


void print_strong_conns(const graph_t *graph) {
    const size_t size = graph_size(graph);
    if (size == 0) {
        return;
    }

    int *names = malloc(size * sizeof(int));
    {
        size_t k = 0;
        const node_t *cur_node = graph->adj_list->head;
        while (cur_node != nullptr) {
            const set_t *cur_set = cur_node->set;
            names[k] = cur_set->edge.from;
            cur_node = cur_node->next;
            ++k;
        }
    }

    // Время первого обнаружения вершины (значение таймера)
    size_t *disc = malloc(size * sizeof(size_t));
    // Самая ранняя вершина, до которой можно добраться из i-й
    size_t *low = malloc(size * sizeof(size_t));
    // Флаги нахождения вершин в стеке
    bool *in_stack = calloc(size, sizeof(bool));
    // Стек накопления вершин текущей компоненты
    size_t *scc_stack = malloc(size * sizeof(size_t));
    size_t scc_top = 0;

    for (size_t i = 0; i < size; ++i) {
        disc[i] = ULLONG_MAX;
        low[i] = ULLONG_MAX;
    }

    // Имитация стека рекурсии DFS
    size_t *call_stack = malloc(size * sizeof(size_t));
    size_t *edge_iter = malloc(size * sizeof(size_t));
    size_t call_top = 0;

    size_t timer = 0;
    size_t comp_num = 0;

    for (size_t start_idx = 0; start_idx < size; ++start_idx) {
        if (disc[start_idx] != ULLONG_MAX) {
            continue;
        }

        call_stack[call_top] = start_idx;
        edge_iter[call_top] = 0;
        ++call_top;

        disc[start_idx] = timer;
        low[start_idx] = timer;
        ++timer;

        scc_stack[scc_top] = start_idx;
        ++scc_top;
        in_stack[start_idx] = true;

        while (call_top > 0) {
            const size_t u_idx = call_stack[call_top - 1];
            size_t *v_idx = &edge_iter[call_top - 1];
            bool descended = false;

            while (*v_idx < size) {
                const size_t next_v = *v_idx;
                *v_idx += 1;

                if (is_edge(graph, names[u_idx], names[next_v])) {
                    if (disc[next_v] == ULLONG_MAX) {
                        disc[next_v] = timer;
                        low[next_v] = timer;
                        ++timer;

                        scc_stack[scc_top] = next_v;
                        ++scc_top;
                        in_stack[next_v] = true;

                        call_stack[call_top] = next_v;
                        edge_iter[call_top] = 0;
                        ++call_top;

                        descended = true;
                        break;
                    }
                    if (in_stack[next_v]) {
                        low[u_idx] = ullmin(low[u_idx], disc[next_v]);
                    }
                }
            }

            if (descended) {
                continue;
            }

            // Выход из вершины (post-visit)
            --call_top;
            if (call_top > 0) {
                const size_t p_idx = call_stack[call_top - 1];
                low[p_idx] = ullmin(low[p_idx], low[u_idx]);
            }

            // Если вершина — корень текущей компоненты сильной связности
            if (low[u_idx] == disc[u_idx]) {
                ++comp_num;
                printf("Component #%llu: ", comp_num);
                bool header = false;

                while (true) {
                    --scc_top;
                    const size_t w_idx = scc_stack[scc_top];
                    in_stack[w_idx] = false;

                    if (!header) {
                        header = true;
                        printf("%d", names[w_idx]);
                    } else {
                        printf(", %d", names[w_idx]);
                    }

                    if (w_idx == u_idx) {
                        break;
                    }
                }
                printf("\n");
            }
        }
    }

    free(edge_iter);
    free(call_stack);
    free(scc_stack);
    free(in_stack);
    free(low);
    free(disc);
    free(names);
}
