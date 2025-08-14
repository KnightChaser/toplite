// src/ui/render.h
#pragma once
#include "../core/process.h"
#include "../core/system.h"

/**
 * @brief Renders the header information for the system status.
 *
 * @param cpu A pointer to a cpu_percent_t structure containing CPU usage data.
 * @param mem A pointer to a mem_info_t structure containing memory data.
 * @param ld A pointer to a load_avg_t structure containing load averages.
 * @param up A pointer to an uptime_fmt_t structure containing system uptime.
 * @param users The number of users currently logged in.
 * @param tc A pointer to a task_counts_t structure containing task counts.
 */
void render_header(const cpu_percent_t *cpu, const mem_info_t *mem,
                   const load_avg_t *ld, const uptime_fmt_t *up, int users,
                   const task_counts_t *tc);

/**
 * @brief Renders the current process information for the main contents.
 *
 * @param list A pointer to a proc_list_t structure containing a process list.
 * @param hz The system clock frequency in Hertz.
 * @param max_rows The maximum number of rows to display in the terminal.
 * @param term_cols The number of columns available in the terminal.
 */
void render_process_list(const proc_list_t *list, long hz,
                         unsigned int max_rows, unsigned int term_cols);
