// src/main.c
#include "core/proc_iter.h"
#include "core/process.h"
#include "core/sorting.h"
#include "core/system.h"
#include "ui/input.h"
#include "ui/render.h"
#include "ui/terminal.h"
#include "util/util.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define HEADER_ROW_COUNT 7

void handle_input(void) {
    char c = '\0';

    if (read(STDIN_FILENO, &c, 1) == 1) {
        switch (c) {
        case 'q':
            // disable_raw_mode() is called by atexit()
            exit(0);
        case '<':
        case ',':
            sorting_prev_column();
            break;
        case '>':
        case '.':
            sorting_next_column();
            break;
        case 'R':
            sorting_flip_direction();
            break;
        }
    }
}

int main(int argc, char **argv) {
    unsigned interval_ms = 2000; // 2 sec

    // If a user wants to specify a different interval, they can do so
    if (argc == 2) {
        char *endptr;
        long val = strtol(argv[1], &endptr, 10);
        // Check for conversion errors and non-numeric input
        if (endptr == argv[1] || *endptr != '\0' || val <= 0) {
            fprintf(stderr, "Usage: %s [interval_ms]\n", argv[0]);
            return 1;
        }
        interval_ms = (unsigned)val;
    } else if (argc > 2) {
        fprintf(stderr, "Usage: %s [interval_ms]\n", argv[0]);
        return 1;
    }

    enable_raw_mode(); // Ready to handle input to change sorting

    cpu_times_t prev_cpu_times = {0}, now_cpu_times = {0};
    mem_info_t mem;
    load_avg_t ld;
    uptime_fmt_t up;
    cpu_percent_t cpu;
    task_counts_t tc;
    terminal_t term;

    proc_list_t proc_list;
    init_process_list(&proc_list);
    long hz = sys_hz();

    // Prime CPU times
    if (read_cpu_times(&prev_cpu_times) != 0) {
        fprintf(stderr, "Error reading initial CPU times(/proc/stat).\n");
        return 1;
    }

    while (true) {
        handle_input(); // Check for user input on each frame

        // Ger the current terminal size
        get_term_size(&term);
        unsigned int available_rows =
            (term.rows > HEADER_ROW_COUNT) ? term.rows - HEADER_ROW_COUNT : 0;
        // Collect data
        read_cpu_times(&now_cpu_times);
        cpu_percent(&prev_cpu_times, &now_cpu_times, &cpu);
        prev_cpu_times = now_cpu_times;

        read_meminfo(&mem);
        read_loadavg(&ld);

        double uptimeSeconds;
        read_uptime(&uptimeSeconds);
        fmt_uptime(uptimeSeconds, &up);

        int users = count_logged_in_users();
        scan_task_states(&tc);

        collect_all_processes(&proc_list, mem.mem_total);

        // --- Sort the process list ---
        qsort(proc_list.procs, proc_list.count, sizeof(proc_info_t),
              compare_procs);

        // --- Render Output ---
        printf("\033[H\033[J"); // Clear screen
        render_header(&cpu, &mem, &ld, &up, users, &tc);
        render_process_list(&proc_list, hz, available_rows, term.cols);
        fflush(stdout);

        usleep(interval_ms * 1000);
    }

    // cleanup
    free_process_list(&proc_list);
    return 0;
}
