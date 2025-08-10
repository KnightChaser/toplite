// src/ui/render.h
#pragma once
#include "../core/system.h"

void render_header_now(const CpuPercentages *cpu, const MemInfo *mem,
                       const LoadAvg *ld, const UptimeFormat *up, int users,
                       const TaskCounts *tc);
