// src/core/sorting.h
#pragma once
#include "process.h"

typedef enum {
    SORT_BY_NONE = 1, // A marker for non-sortable columns
    SORT_BY_PID,
    SORT_BY_CPU,
    SORT_BY_MEM,
    SORT_BY_TIME,
    SORT_BY_COMMAND,
    SORT_COLUMN_COUNT, // How many modes?
} sort_by_t;

typedef enum {
    ASCENDING = 1,
    DESCENDING = -1,
} sort_direction_t;

/**
 * @brief Change the current sorting column to the next one.
 *        (e.g. SORT_BY_PID(0) -> SORT_BY_CPU(1) -> SORT_BY_MEM(2) ...)
 */
void sorting_next_column(void);

/**
 * @brief Change the current sorting column to the previous one.
 *        (e.g. SORT_BY_MEM(2) -> SORT_BY_CPU(1) -> SORT_BY_PID(0) ...)
 */
void sorting_prev_column(void);

/**
 * @brief Flip the current sorting direction.
 *        (e.g. ASCENDING -> DESCENDING, DESCENDING -> ASCENDING)
 */
void sorting_flip_direction(void);

/**
 * @brief Get the current sorting column.
 * @return The current sort_by_t value.
 */
sort_by_t sorting_get_current_column(void);

/**
 * @brief The comparison function used by qsort
 * @details It is the core of the sorting logic. It compares two proc_info_t
 *          structs based on the global g_sort_by setting.
 *
 * @param a Pointer to the first proc_info_t struct.
 * @param b Pointer to the second proc_info_t struct.
 * @return < 0 if a should come before b,
 *         > 0 if a should come after b,
 *         0 if they are equal.
 */
int compare_procs(const void *a, const void *b);
