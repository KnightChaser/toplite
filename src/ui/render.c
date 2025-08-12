// src/ui/render.c
#include "render.h"
#include "color.h"
#include <stdio.h>
#include <time.h>

/**
 * Prints the CPU percentages in a formatted manner.
 *
 * @param cpu A pointer to a CpuPercentages structure containing CPU usage data.
 */
static void print_mem(const MemInfo *mem) {
    // The "buff/cache" line in 'top' is a sum of Buffers, Cached, and
    // SReclaimable, which represents memory that can be quickly freed
    // by the kernel for application use. Shmem is subtracted as it is
    // not considered easily reclaimable in the same way.
    unsigned long long buffcache =
        mem->buffers + mem->cached + mem->sreclaimable - mem->shmem;
    unsigned long long used = mem->mem_total - mem->mem_free - buffcache;
    unsigned long long swap_used = mem->swap_total - mem->swap_free;

    printf("KiB Mem : " BOLD "%8llu" RESET " total, " BOLD "%8llu" RESET
           " free, " BOLD "%8llu" RESET " used, " BOLD "%8llu" RESET
           " buff/cache\n",
           mem->mem_total, mem->mem_free, used, buffcache);

    printf("KiB Swap: " BOLD "%8llu" RESET " total, " BOLD "%8llu" RESET
           " free, " BOLD "%8llu" RESET " used, " BOLD "%8llu" RESET
           " avail Mem\n",
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
    char tbuf[32] = {0};
    time_t t = time(NULL);
    struct tm localtime;
    localtime_r(&t, &localtime);
    strftime(tbuf, sizeof tbuf, "%H:%M:%S", &localtime);

    printf("toplite - %s up ", tbuf);
    if (up->days > 0) {
        printf(BOLD "%u" RESET " days, ", up->days);
    }
    printf(BOLD "%u:%02u" RESET ",  " BOLD "%d" RESET
                " users,  load average: " BOLD "%.2f" RESET ", " BOLD
                "%.2f" RESET ", " BOLD "%.2f" RESET "\n",
           up->hours, up->minutes, users, ld->load1, ld->load5, ld->load15);

    // line 2: tasks
    printf("Tasks: " BOLD "%u" RESET " total,   " BOLD "%u" RESET
           " running, " BOLD "%u" RESET " sleeping, " BOLD "%u" RESET
           " stopped, " BOLD "%u" RESET " zombie\n",
           tc->total, tc->running, tc->sleeping, tc->stopped, tc->zombie);

    // line 3: CPU
    printf("%%Cpu(s): " BOLD "%4.1f" RESET " us, " BOLD "%4.1f" RESET
           " sy, " BOLD "%4.1f" RESET " ni, " BOLD "%4.1f" RESET " id, " BOLD
           "%4.1f" RESET " wa, " BOLD "%4.1f" RESET " hi, " BOLD "%4.1f" RESET
           " si, " BOLD "%4.1f" RESET " st\n",
           cpu->us, cpu->sy, cpu->ni, cpu->id, cpu->wa, cpu->hi, cpu->si,
           cpu->st);

    print_mem(mem);
}
