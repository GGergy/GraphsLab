#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <limits.h>

#include "graph.h"
#include "map.h"


// Типы эвристик
typedef enum HeuristicType {
    HEURISTIC_EUCLIDEAN,
    HEURISTIC_MANHATTAN
} heuristic_t;

// Вычисление эвристики
static double calculate_heuristic(const int r, const int c, const int end_r, const int end_c, const heuristic_t type) {
    const double dr = abs(r - end_r);
    const double dc = abs(c - end_c);

    if (type == HEURISTIC_MANHATTAN) {
        return dr + dc;
    }
    return sqrt(dr * dr + dc * dc);
}

// Структура результата работы алгоритма
typedef struct AStarResult {
    path_t path;
    double visited_percentage;
    double elapsed_ms;
    bool found;
} astar_result_t;

// Алгоритм A* в вашем стиле: линейный поиск минимума по массиву без сторонних очередей
static astar_result_t astar_search(const graph_t *graph, const int cols,
                                   const point_t start, const point_t end, const heuristic_t h_type) {
    astar_result_t res;
    res.found = false;
    res.path.path = nullptr;
    res.path.length = 0;
    res.path.total_dist = 0;
    res.visited_percentage = 0.0;
    res.elapsed_ms = 0.0;

    const size_t size = graph_size(graph);
    if (size == 0) return res;

    const int start_u = start.r * cols + start.c;
    const int end_u = end.r * cols + end.c;

    if (!is_vertex(graph, start_u) || !is_vertex(graph, end_u)) {
        return res;
    }

    const clock_t t_start = clock();

    size_t *dist = malloc(size * sizeof(size_t));
    bool *worked = malloc(size * sizeof(bool));
    int *names = malloc(size * sizeof(int));
    int *parent = malloc(size * sizeof(int)); // ~Из какой вершины пришли в i-ю для восстановления пути

    // Инициализация имен вершин из связного списка графа
    {
        size_t k = 0;
        const node_t *cur_node = graph->adj_list->head;
        while (cur_node != nullptr) {
            names[k] = cur_node->set->edge.from;
            cur_node = cur_node->next;
            ++k;
        }
    }

    size_t start_index = size;
    size_t end_index = size;
    for (size_t i = 0; i < size; ++i) {
        worked[i] = false;
        dist[i] = ULLONG_MAX;
        parent[i] = -1;
        if (names[i] == start_u) start_index = i;
        if (names[i] == end_u) end_index = i;
    }

    if (start_index == size || end_index == size) {
        free(dist);
        free(worked);
        free(names);
        free(parent);
        return res;
    }

    dist[start_index] = 0;
    size_t visited_count = 0;

    while (true) {
        double min_f = INFINITY;
        size_t min_index = size;

        // Линейный поиск вершины с минимальным f = dist[i] + h(names[i])
        for (size_t i = 0; i < size; ++i) {
            if (!worked[i] && dist[i] != ULLONG_MAX) {
                const int r = names[i] / cols;
                const int c = names[i] % cols;
                const double h = calculate_heuristic(r, c, end.r, end.c, h_type);
                const double f = (double)dist[i] + h;

                if (f < min_f) {
                    min_f = f;
                    min_index = i;
                }
            }
        }

        if (min_index == size) {
            break; // Нет достижимых непосещенных вершин
        }

        worked[min_index] = true;
        visited_count++;

        if (min_index == end_index) {
            // Досрочная остановка после достижения цели
            res.found = true;
            break;
        }

        // Обновление расстояний до соседей
        const set_t *u_set = find_set(graph->adj_list, names[min_index]);
        if (u_set != nullptr) {
            const set_t *cur_edge = u_set->next;
            while (cur_edge != nullptr) {
                const int to_vertex = cur_edge->edge.to;
                const int weight = cur_edge->edge.weight;

                // Находим индекс соседа
                for (size_t j = 0; j < size; ++j) {
                    if (names[j] == to_vertex) {
                        if (!worked[j]) {
                            const size_t new_dist = dist[min_index] + (size_t)weight;
                            if (new_dist < dist[j]) {
                                dist[j] = new_dist;
                                parent[j] = (int)min_index;
                            }
                        }
                        break;
                    }
                }
                cur_edge = cur_edge->next;
            }
        }
    }

    const clock_t t_end = clock();
    res.elapsed_ms = (double)(t_end - t_start) * 1000.0 / CLOCKS_PER_SEC;

    res.visited_percentage = (double)visited_count / (double)size * 100.0;

    if (res.found) {
        res.path.total_dist = dist[end_index];

        // Восстановление пути
        size_t len = 0;
        for (int curr = (int)end_index; curr != -1; curr = parent[curr]) {
            len++;
            if ((size_t)curr == start_index) break;
        }

        res.path.length = len;
        res.path.path = malloc(len * sizeof(int));
        int curr = (int)end_index;
        for (size_t i = len; i > 0; --i) {
            res.path.path[i - 1] = names[curr];
            if ((size_t)curr == start_index) break;
            curr = parent[curr];
        }
    }

    free(dist);
    free(worked);
    free(names);
    free(parent);

    return res;
}

// Запись блока результатов для одной эвристики
static void write_heuristic_report(FILE *file, const char *heuristic_name, const astar_result_t *res,
                                   const point_t start, const point_t end, const int cols) {
    fprintf(file, "=== A* algorithm (%s) ===\n", heuristic_name);
    if (!res->found) {
        fprintf(file, "Path between (%d, %d) and (%d, %d) not found!\n",
                start.r, start.c, end.r, end.c);
        fprintf(file, "Visited vertices: %.2f%%\n", res->visited_percentage);
        fprintf(file, "Elapsed time: %.4f ms\n\n", res->elapsed_ms);
        return;
    }

    fprintf(file, "%zu - total dist of path between (%d, %d) and (%d, %d) points.\n",
            res->path.total_dist, start.r, start.c, end.r, end.c);
    fprintf(file, "Path:\n[");
    for (size_t i = 0; i < res->path.length; ++i) {
        const int u = res->path.path[i];
        const int r = u / cols;
        const int c = u % cols;
        fprintf(file, "(%d, %d)", r, c);
        if (i + 1 < res->path.length) {
            fprintf(file, ", ");
        }
    }
    fprintf(file, "]\n");
    fprintf(file, "Visited vertices: %.2f%%\n", res->visited_percentage);
    fprintf(file, "Elapsed time: %.4f ms\n\n", res->elapsed_ms);
}

void run_map(const char *fin, const point_t start, const point_t end, const char *fout) {
    FILE *file = fopen(fin, "r");
    if (file == nullptr) {
        printf("Error: Could not open map file %s\n", fin);
        return;
    }

    int rows = 0;
    int cols = 0;
    if (fscanf(file, "%d %d", &rows, &cols) != 2 || rows <= 0 || cols <= 0) {
        printf("Error: Invalid map dimensions in %s\n", fin);
        fclose(file);
        return;
    }

    int *heights = malloc(rows * cols * sizeof(int));
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            if (fscanf(file, "%d", &heights[r * cols + c]) != 1) {
                printf("Error reading height at (%d, %d)\n", r, c);
                free(heights);
                fclose(file);
                return;
            }
        }
    }
    fclose(file);

    // Проверка граничных условий для start и end
    if (start.r < 0 || start.r >= rows || start.c < 0 || start.c >= cols ||
        end.r < 0 || end.r >= rows || end.c < 0 || end.c >= cols) {
        printf("Error: Coordinates are out of map bounds!\n");
        free(heights);
        return;
    }

    const int start_h = heights[start.r * cols + start.c];
    const int end_h = heights[end.r * cols + end.c];
    if (start_h == 0 || end_h == 0) {
        printf("Error: Start or end point is an obstacle (height 0)!\n");
        free(heights);
        return;
    }

    // Создаем неориентированный граф
    graph_t *graph = allocate_graph(false);

    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            if (heights[r * cols + c] > 0) {
                // ~Добавляем в граф все проходимые вершины h>0
                add_vertex(graph, r * cols + c);
            }
        }
    }

    // Добавляем ребра между проходимыми соседними клетками (4 направления)
    // ~Сдвиги по row и по col
    constexpr int dr[4] = {-1, 1, 0, 0};
    constexpr int dc[4] = {0, 0, -1, 1};

    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            const int u = r * cols + c;
            const int h_u = heights[u];
            if (h_u == 0) continue;

            for (int k = 0; k < 4; ++k) {
                // ~Проход по всем сдвигам - вверх, вниз, влево, вправо
                const int nr = r + dr[k];
                const int nc = c + dc[k];
                if (nr >= 0 && nr < rows && nc >= 0 && nc < cols) {
                    // ~Проверка выхода за границы и проходимости вершины
                    const int v = nr * cols + nc;
                    const int h_v = heights[v];
                    if (h_v > 0) {
                        // ~Вес ребра - стоимость перехода в соседнюю клетку (1) + разница в высотах dh
                        const int weight = 1 + abs(h_u - h_v);
                        if (!is_edge(graph, u, v)) {
                            add_edge(graph, u, v, weight);
                        }
                    }
                }
            }
        }
    }

    // Запуск A* с Манхэттенской эвристикой
    const astar_result_t res_manhattan = astar_search(graph, cols, start, end, HEURISTIC_MANHATTAN);

    // Запуск A* с Евклидовой эвристикой
    const astar_result_t res_euclidean = astar_search(graph, cols, start, end, HEURISTIC_EUCLIDEAN);

    // Запись отчета в fout
    FILE *out_file = fopen(fout, "w");
    if (out_file == nullptr) {
        printf("Error: Could not open output file %s\n", fout);
    } else {
        write_heuristic_report(out_file, "Manhattan heuristic", &res_manhattan, start, end, cols);
        write_heuristic_report(out_file, "Euclidean heuristic", &res_euclidean, start, end, cols);
        fclose(out_file);
        printf("Map search finished successfully. Result saved to %s\n", fout);
    }

    if (res_manhattan.path.path != nullptr) free(res_manhattan.path.path);
    if (res_euclidean.path.path != nullptr) free(res_euclidean.path.path);

    free(heights);
    free_graph(graph);
}
