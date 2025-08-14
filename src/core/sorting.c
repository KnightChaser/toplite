// src/core/sorting.c
#include "sorting.h"
#include <string.h>

// Global variables to control sorting state.
static sort_by_t g_sort_by = SORT_BY_CPU;
static sort_direction_t g_sort_direction = DESCENDING;

void sorting_next_column(void) {
    g_sort_by = (g_sort_by + 1) % SORT_COLUMN_COUNT;
}

void sorting_prev_column(void) {
    g_sort_by = (g_sort_by == 0) ? (SORT_COLUMN_COUNT - 1) : (g_sort_by - 1);
}

void sorting_flip_direction(void) {
    g_sort_direction = (g_sort_direction == ASCENDING) ? DESCENDING : ASCENDING;
}

sort_by_t sorting_get_current_column(void) { return g_sort_by; }

int compare_procs(const void *a, const void *b) {
    const proc_info_t *p1 = (const proc_info_t *)a;
    const proc_info_t *p2 = (const proc_info_t *)b;
    int result = 0;

    switch (g_sort_by) {
    case SORT_BY_CPU:
        // For numerical values, descending order is usually preferred.
        if (p2->cpu_percent > p1->cpu_percent)
            result = 1;
        if (p2->cpu_percent < p1->cpu_percent)
            result = -1;
        break;

    case SORT_BY_MEM:
        if (p2->mem_percent > p1->mem_percent)
            result = 1;
        if (p2->mem_percent < p1->mem_percent)
            result = -1;
        break;

    case SORT_BY_TIME:
        if (p2->uptime_ticks > p1->uptime_ticks)
            result = 1;
        if (p2->uptime_ticks < p1->uptime_ticks)
            result = -1;
        break;

    case SORT_BY_PID:
        // For PID, ascending order is standard
        result = p1->pid - p2->pid;
        break;

    case SORT_BY_COMMAND:
        // For strings, use standard string comparison.
        result = strncmp(p1->command, p2->command, sizeof(p1->command));
        break;

    default:
        result = 0;
    }

    // Apply the global sort direction(g_sort_direction)
    return result * g_sort_direction;
}
