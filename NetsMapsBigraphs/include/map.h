//
// Created by Gergy on 11.11.2026.
//
#ifndef NETSMAPSBIGRAPHS_MAP_H
#define NETSMAPSBIGRAPHS_MAP_H

#pragma once

typedef struct Point {
    int r;
    int c;
} point_t;

void run_map(const char *fin, point_t start, point_t end, const char *fout);

#endif //NETSMAPSBIGRAPHS_MAP_H
