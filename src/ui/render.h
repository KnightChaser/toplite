// src/ui/render.h
#pragma once
#include "../core/process.h"
#include "../core/system.h"

/**
 * @brief Prints memory accounting information in a formatted manner.
 *
 * @param mem A pointer to a mem_info_t structure containing memory data.
 */
void render_header_now(const cpu_percent_t *cpu, const mem_info_t *mem,
                       const load_avg_t *ld, const uptime_fmt_t *up, int users,
                       const task_counts_t *tc);

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
void render_process_list(const proc_list_t *list, long hz,
                         unsigned int max_rows);
