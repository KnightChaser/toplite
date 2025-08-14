// src/ui/render.c
#include "render.h"
#include "../core/sorting.h"
#include "color.h"
#include <stdio.h>
#include <time.h>

// Define a max width for the command column for clearer UI output
#define COMMAND_WIDTH 40

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

void render_header(const cpu_percent_t *cpu, const mem_info_t *mem,
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

void render_process_list(const proc_list_t *list, long hz,
                         unsigned int max_rows, unsigned int term_cols) {
    if (!list || hz <= 0 || max_rows == 0) {
        return;
    }

    // A correct mapping for visual columns to sort type
    const sort_by_t column_map[] = {
        SORT_BY_PID,    // Col 0: PID
        SORT_BY_NONE,   // Col 1: USER
        SORT_BY_NONE,   // Col 2: PR
        SORT_BY_NONE,   // Col 3: NI
        SORT_BY_NONE,   // Col 4: VIRT
        SORT_BY_NONE,   // Col 5: RES
        SORT_BY_NONE,   // Col 6: SHR
        SORT_BY_NONE,   // Col 7: S
        SORT_BY_CPU,    // Col 8: %CPU
        SORT_BY_MEM,    // Col 9: %MEM
        SORT_BY_TIME,   // Col 10: TIME+
        SORT_BY_COMMAND // Col 11: COMMAND
    };

    // Build and print the styled, highlighted header
    sort_by_t current_sort = sorting_get_current_column();
    char header_buf[512] = {0};
    int offset = 0;

    // An array of column names and their widths
    const char *cols[] = {"PID", "USER", "PR",   "NI",   "VIRT",  "RES",
                          "SHR", "S",    "%CPU", "%MEM", "TIME+", "COMMAND"};
    const int widths[] = {5, -8, 3, 3, 8, 8, 8, 1, 5, 5, 9, -COMMAND_WIDTH};

    // Print the header with appropriate styles
    for (int i = 0; i < (int)(sizeof(cols) / sizeof(cols[0])); i++) {
        const char *style = BOLD;

        // Highlight if the column is sortable AND it's the current sort
        if (column_map[i] != SORT_BY_NONE && column_map[i] == current_sort) {
            style = BOLD REVERSE;
        }

        offset += snprintf(header_buf + offset, sizeof(header_buf) - offset,
                           "%s%*s" RESET, style, widths[i], cols[i]);
        if (i < 11) {
            offset +=
                snprintf(header_buf + offset, sizeof(header_buf) - offset, " ");
        }
    }

    // Print the full header, padded to the screen width
    printf("%-*s\n", term_cols, header_buf);

    // Cut output if it exceeds the terminal size
    size_t render_count = list->count > max_rows ? max_rows : list->count;
    for (size_t i = 0; i < render_count; i++) {
        const proc_info_t *p = &list->procs[i];

        // Format TIME+ from clock ticks to MM:SS.ss
        unsigned long long total_seconds = p->uptime_ticks / hz;
        unsigned long minutes = total_seconds / 60;
        char time_str[16] = {0};
        snprintf(time_str, sizeof(time_str), "%lu:%02llu.%02llu", minutes,
                 total_seconds % 60, (p->uptime_ticks % hz) * 100 / hz);

        printf("%5d %-8.8s %3.3s %3ld %8lu %8lu %8lu %c %5.1f %5.1f %9s "
               "%-.*s\n",
               p->pid, p->user, p->priority, p->nice, p->virt_mem, p->res_mem,
               p->shr_mem, p->state, p->cpu_percent, p->mem_percent, time_str,
               COMMAND_WIDTH, p->command);
    }
}
