// src/core/system.c
#include "system.h"
#include "../util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <utmp.h>

/**
 * Reads the system's memory information from /proc/meminfo.
 * Populates the MemInfo structure with the data read.
 *
 * @param cpu_times Pointer to a CpuTimes structure to populate.
 * @return 0 on success, -1 on failure.
 */
int read_cpu_times(CpuTimes *cpu_times) {
    char *buf;
    size_t bytes_read;
    if (!read_text_file("/proc/stat", &buf, &bytes_read)) {
        return -1;
    }

    const char *p = buf;
    if (strncmp(p, "cpu ", 4) != 0) {
        // Not the expected format
        free(buf);
        return -1;
    }

    p += 4; // Skip "cpu "
    cpu_times->user = strtoull_safe(&p);
    cpu_times->nice = strtoull_safe(&p);
    cpu_times->system = strtoull_safe(&p);
    cpu_times->idle = strtoull_safe(&p);
    cpu_times->iowait = strtoull_safe(&p);
    cpu_times->irq = strtoull_safe(&p);
    cpu_times->softirq = strtoull_safe(&p);
    cpu_times->steal = strtoull_safe(&p);
    cpu_times->guest = strtoull_safe(&p);
    cpu_times->guest_nice = strtoull_safe(&p);

    free(buf);
    return 0;
}

/**
 * Reads the system's memory information from /proc/meminfo.
 * Populates the MemInfo structure with the data read.
 *
 * @param mem_info Pointer to a MemInfo structure to populate.
 * @return 0 on success, -1 on failure.
 */
int read_meminfo(MemInfo *mem_info) {
    char *buf;
    size_t bytes_read;
    if (!read_text_file("/proc/meminfo", &buf, &bytes_read)) {
        return -1;
    }

    // Simple line scan
    char *save, *line = strtok_r(buf, "\n", &save);
    memset(mem_info, 0, sizeof(MemInfo));
    while (line) {
        // Keep MemInfo in KiB to match "KiB Mem" output
        if (sscanf(line, "MemTotal: %llu kB", &mem_info->mem_total) == 1) {
        } else if (sscanf(line, "MemFree: %llu kB", &mem_info->mem_free) == 1) {
        } else if (sscanf(line, "Buffers: %llu kB", &mem_info->buffers) == 1) {
        } else if (sscanf(line, "Cached: %llu kB", &mem_info->cached) == 1) {
        } else if (sscanf(line, "SReclaimable: %llu kB",
                          &mem_info->sreclaimable) == 1) {
        } else if (sscanf(line, "Shmem: %llu kB", &mem_info->shmem) == 1) {
        } else if (sscanf(line, "MemAvailable: %llu kB",
                          &mem_info->mem_available) == 1) {
        } else if (sscanf(line, "SwapTotal: %llu kB", &mem_info->swap_total) ==
                   1) {
        } else if (sscanf(line, "SwapFree: %llu kB", &mem_info->swap_free) ==
                   1) {
        }

        line = strtok_r(NULL, "\n", &save);
    }

    free(buf);
    return 0;
}

/**
 * Reads the system's uptime from /proc/uptime and formats it into days, hours,
 * and minutes.
 *
 * @param uptime Pointer to a UptimeFormat structure to populate.
 * @return 0 on success, -1 on failure.
 */
int read_loadavg(LoadAvg *load_avg) {
    FILE *f = fopen("/proc/loadavg", "r");
    if (!f) {
        return -1;
    }

    if (fscanf(f, "%lf %lf %lf", &load_avg->load1, &load_avg->load5,
               &load_avg->load15) != 3) {
        fclose(f);
        return -1;
    }

    fclose(f);
    return 0;
}

/**
 * Reads the system's uptime from /proc/uptime and returns it in seconds.
 *
 * @param uptime_sec Pointer to a double to store the uptime in seconds.
 * @return 0 on success, -1 on failure.
 */
int read_uptime(double *uptime_sec) {
    FILE *f = fopen("/proc/uptime", "r");
    if (!f) {
        return -1;
    }

    if (fscanf(f, "%lf", uptime_sec) != 1) {
        fclose(f);
        return -1;
    }

    fclose(f);
    return 0;
}

/**
 * Counts the number of logged-in users by reading the utmp entries.
 *
 * @return The count of logged-in users.
 */
int count_logged_in_users(void) {
    int count = 0;
    setutent();
    struct utmp *u;
    while ((u = getutent()) != NULL) {
        // Iterate through utmp entries
        // Count only user processes with a non-empty username
        if (u->ut_type == USER_PROCESS && u->ut_user[0]) {
            count++;
        }
    }
    endutent();
    return count;
}

/**
 * Simply calculates the total jiffies from the CpuTimes structure.
 * This is the sum of all CPU time fields.
 *
 * @param cpu_times Pointer to a CpuTimes structure.
 * @return The total jiffies as an unsigned long long.
 */
static unsigned long long total_jiffies(const CpuTimes *cpu_times) {
    return cpu_times->user + cpu_times->nice + cpu_times->system +
           cpu_times->idle + cpu_times->iowait + cpu_times->irq +
           cpu_times->softirq + cpu_times->steal + cpu_times->guest +
           cpu_times->guest_nice;
}

/**
 * Calculates the CPU usage percentages based on the previous and current CPU
 * times.
 *
 * @param prev Pointer to the previous CpuTimes structure.
 * @param now Pointer to the current CpuTimes structure.
 * @param percentages Pointer to a CpuPercentages structure to populate with
 *                    calculated percentages.
 */
void cpu_percent(const CpuTimes *prev, const CpuTimes *now,
                 CpuPercentages *percentages) {
    double total = (double)(total_jiffies(now) - total_jiffies(prev));
    if (total <= 0) {
        // If total is zero or negative, we cannot calculate percentages
        // set all percentages to zero and id to 100%
        memset(percentages, 0, sizeof(CpuPercentages));
        percentages->id = 100.0; // 100 percent idle
        return;
    }

#define D(field) ((double)(now->field - prev->field))

    percentages->us = (D(user) / total) * 100.0;
    percentages->sy = (D(system) / total) * 100.0;
    percentages->ni = (D(nice) / total) * 100.0;
    percentages->id = (D(idle) / total) * 100.0;
    percentages->wa = (D(iowait) / total) * 100.0;
    percentages->hi = (D(irq) / total) * 100.0;
    percentages->si = (D(softirq) / total) * 100.0;
    percentages->st = (D(steal) / total) * 100.0;

#undef D
}

/**
 * Formats the uptime in seconds into days, hours, and minutes.
 *
 * @param up The uptime in seconds.
 * @param o Pointer to a UptimeFormat structure to populate with formatted
 *          uptime.
 */
void fmt_uptime(double up, UptimeFormat *o) {
    unsigned long mins = (unsigned long)(up / 60);
    o->days = mins / (60 * 24);
    o->hours = (mins / 60) % 24;
    o->minutes = mins % 60;
}
