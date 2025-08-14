// src/core/proc_iter.h
#pragma once
#include "system.h"

/**
 * @brief Scan the /proc filesystem to count the number of tasks in various
 * states.
 *
 * @param out Pointer to a TaskCounts structure to fill with the counts.
 * @return 0 on success, -1 on error (e.g., if /proc cannot be opened).
 */
int scan_task_states(task_counts_t *out);
