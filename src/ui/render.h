// src/ui/render.h
#pragma once
#include "../core/process.h"
#include "../core/system.h"

void render_header_now(const CpuPercentages *cpu, const MemInfo *mem,
                       const LoadAvg *ld, const UptimeFormat *up, int users,
                       const TaskCounts *tc);

void render_process_list(const ProcessList *list, long hz);
