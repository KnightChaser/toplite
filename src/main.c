// src/main.c
#include "core/proc_iter.h"
#include "core/system.h"
#include "ui/render.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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

    CpuTimes prev_cpu_times = {0}, now_cpu_times = {0};
    MemInfo mem;
    LoadAvg ld;
    double uptimeSeconds = 0.0;
    UptimeFormat up;
    CpuPercentages cpu;
    TaskCounts tc;

    // Prime CPU times
    if (read_cpu_times(&prev_cpu_times) != 0) {
        fprintf(stderr, "Error reading initial CPU times(/proc/stat).\n");
        return 1;
    }

    while (true) {
        usleep(interval_ms * 1000);

        if (read_cpu_times(&now_cpu_times) != 0) {
            // Error reading CPU times
            continue;
        }

        cpu_percent(&prev_cpu_times, &now_cpu_times, &cpu);
        prev_cpu_times = now_cpu_times;

        if (read_meminfo(&mem) != 0) {
            continue;
        }
        if (read_loadavg(&ld) != 0) {
            continue;
        }
        if (read_uptime(&uptimeSeconds) != 0) {
            continue;
        }

        fmt_uptime(uptimeSeconds, &up);
        int users = count_logged_in_users();

        if (scan_task_states(&tc) != 0) {
            continue;
        }

        // Clear screen like top command, dead simple
        printf("\033[H\033[J");
        render_header_now(&cpu, &mem, &ld, &up, users, &tc);
        fflush(stdout);
    }
}
