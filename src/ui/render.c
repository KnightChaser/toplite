// src/ui/render.c
#include "render.h"
#include "color.h"
#include <stdio.h>
#include <time.h>

// Define a max width for the command column for clearer UI output
#define COMMAND_WIDTH 40

/**
 * Prints memory accounting information in a formatted manner.
 *
 * @param mem A pointer to a mem_info_t structure containing memory data.
 */
static void print_mem(const mem_info_t *mem) {
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
 * @param cpu A pointer to a cpu_percent_t structure containing CPU usage data.
 * @param mem A pointer to a mem_info_t structure containing memory data.
 * @param ld A pointer to a load_avg_t structure containing load averages.
 * @param up A pointer to an uptime_fmt_t structure containing system uptime.
 * @param users The number of users currently logged in.
 * @param tc A pointer to a task_counts_t structure containing task counts.
 */
void render_header_now(const cpu_percent_t *cpu, const mem_info_t *mem,
                       const load_avg_t *ld, const uptime_fmt_t *up, int users,
                       const task_counts_t *tc) {
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

/**
 * Renders the process list in a formatted manner.
 *
 * @param list A pointer to a proc_list_t structure containing process data.
 * @param hz The number of clock ticks per second (Hertz).
 */
void render_process_list(const proc_list_t *list, long hz) {
    if (!list || hz <= 0) {
        return;
    }

    // Print the header for the process list
    printf("%*s %-8s %3s %3s %8s %8s %8s %c %5s %5s %9s %-*s\n", 5, "PID",
           "USER", "PR", "NI", "VIRT", "RES", "SHR", 'S', "%CPU", "%MEM",
           "TIME+", COMMAND_WIDTH, "COMMAND");

    for (size_t i = 0; i < list->count; ++i) {
        const proc_info_t *p = &list->procs[i];

        // Format TIME+ from clock ticks to MM:SS.ss
        unsigned long long total_seconds = p->uptime_ticks / hz;
        unsigned long minutes = total_seconds / 60;
        char time_str[16] = {0};
        snprintf(time_str, sizeof(time_str), "%lu:%02llu.%02llu", minutes,
                 total_seconds % 60, (p->uptime_ticks % hz) * 100 / hz);

        printf("%5d %-8.8s %3.3s %3d %8lu %8lu %8lu %c %5.1f %5.1f %9s %-.*s\n",
               p->pid, p->user, p->priority, p->nice, p->virt_mem, p->res_mem,
               p->shr_mem, p->state, p->cpu_percent, p->mem_percent, time_str,
               COMMAND_WIDTH, p->command);
    }
}
