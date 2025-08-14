// src/ui/render.h
#pragma once
#include "../core/process.h"
#include "../core/system.h"

void render_header_now(const cpu_percent_t *cpu, const mem_info_t *mem,
                       const load_avg_t *ld, const uptime_fmt_t *up, int users,
                       const task_counts_t *tc);

void render_process_list(const proc_list_t *list, long hz);
