// src/ui/render.c
#include "render.h"
#include <stdio.h>
#include <time.h>

/**
 * Prints the CPU percentages in a formatted manner.
 *
 * @param cpu A pointer to a CpuPercentages structure containing CPU usage data.
 */
static void print_mem(const MemInfo *mem) {
    // Match top's math:
    unsigned long long buffcache =
        mem->buffers + mem->cached + mem->sreclaimable - mem->shmem;
    unsigned long long used = mem->mem_total - mem->mem_free - buffcache;

    printf("KiB Mem : %7llu total, %7llu free, %7llu used, %7llu buff/cache\n",
           mem->mem_total, mem->mem_free, used, buffcache);
    unsigned long long swap_used = mem->swap_total - mem->swap_free;
    printf("KiB Swap: %7llu total, %7llu free, %7llu used, %7llu avail Mem\n",
           mem->swap_total, mem->swap_free, swap_used, mem->mem_available);
}

/**
 * Renders the header information for the system status.
 *
 * @param cpu A pointer to a CpuPercentages structure containing CPU usage data.
 * @param mem A pointer to a MemInfo structure containing memory information.
 * @param ld A pointer to a LoadAvg structure containing load averages.
 * @param up A pointer to a UptimeFormat structure containing uptime
 * information.
 * @param users The number of users currently logged in.
 * @param tc A pointer to a TaskCounts structure containing task counts.
 */
void render_header_now(const CpuPercentages *cpu, const MemInfo *mem,
                       const LoadAvg *ld, const UptimeFormat *up, int users,
                       const TaskCounts *tc) {
    // line 1: time, uptime, users, loadavg
    char tbuf[32] = {0};
    time_t t = time(NULL);
    struct tm localtime;
    localtime_r(&t, &localtime);
    strftime(tbuf, sizeof tbuf, "%H:%M:%S", &localtime);
    printf("top - %s up %u days, %u:%02u,  %d users,  load average: %.2f, "
           "%.2f, %.2f\n",
           tbuf, up->days, up->hours, up->minutes, users, ld->load1, ld->load5,
           ld->load15);

    // line 2: tasks
    printf(
        "Tasks: %u total,   %u running, %u sleeping, %u stopped, %u zombie\n",
        tc->total, tc->running, tc->sleeping, tc->stopped, tc->zombie);

    // line 3: CPU
    printf("%%Cpu(s): %4.1f us, %4.1f sy, %4.1f ni, %4.1f id, %4.1f wa, %4.1f "
           "hi, %4.1f si, %4.1f st\n",
           cpu->us, cpu->sy, cpu->ni, cpu->id, cpu->wa, cpu->hi, cpu->si,
           cpu->st);

    // lines 4-5: memory
    print_mem(mem);
}
