// src/core/system.h
#pragma once
#include <stdint.h>

/**
 * A structure that represents the system's CPU times.
 * It contains various fields for different CPU states.
 */
typedef struct {
    unsigned long long user;       // User time (normal processes)
    unsigned long long nice;       // User time (nice processes)
    unsigned long long system;     // System time (kernel processes)
    unsigned long long idle;       // Idle time
    unsigned long long iowait;     // I/O wait time
    unsigned long long irq;        // Hardware interrupt time
    unsigned long long softirq;    // Software interrupt time
    unsigned long long steal;      // Time stolen from a virtual machine
    unsigned long long guest;      // Guest time
    unsigned long long guest_nice; // Guest time (nice processes)
} cpu_times_t;

/**
 * A structure that represents the system's memory information.
 * It contains fields for total, free, and available memory, as well as
 * swap space information.
 */
typedef struct {
    unsigned long long mem_total;     // Total memory in bytes
    unsigned long long mem_free;      // Free memory in bytes
    unsigned long long buffers;       // Memory used for buffers
    unsigned long long cached;        // Memory used for cache
    unsigned long long sreclaimable;  // Memory reclaimable from slab
    unsigned long long shmem;         // Shared memory
    unsigned long long mem_available; // Available memory in bytes
    unsigned long long swap_total;    // Total swap space in bytes
    unsigned long long swap_free;     // Free swap space in bytes
} mem_info_t;

/**
 * A structure that represents the system's load averages.
 * It contains fields for the load averages over the last 1, 5, and 15 minutes.
 */
typedef struct {
    double load1;  // Load average for the last 1 minute
    double load5;  // Load average for the last 5 minutes
    double load15; // Load average for the last 15 minutes
} load_avg_t;

/**
 * A structure that represents the system's uptime.
 * It contains fields for the number of days, hours, and minutes the system has
 * been up.
 */
typedef struct {
    unsigned days;    // Number of days
    unsigned hours;   // Number of hours
    unsigned minutes; // Number of minutes
} uptime_fmt_t;

/**
 * A structure that represents the CPU usage statistics.
 * It contains fields for user, system, nice, idle, wait, hardware interrupt,
 * software interrupt, and steal times.
 */
typedef struct {
    double us; // User time percentage
    double sy; // System time percentage
    double ni; // Nice time percentage
    double id; // Idle time percentage
    double wa; // I/O wait time percentage
    double hi; // Hardware interrupt time percentage
    double si; // Software interrupt time percentage
    double st; // Steal time percentage
} cpu_percent_t;

/**
 * A structure that represents the counts of various task states in the system.
 * It contains fields for total tasks, running tasks, sleeping tasks, stopped
 * tasks, and zombie tasks.
 */
typedef struct {
    unsigned total;
    unsigned running;
    unsigned sleeping;
    unsigned stopped;
    unsigned zombie;
} task_counts_t;

/**
 * @brief Reads the system's memory information from /proc/meminfo.
 * And, it populates the MemInfo structure with the data read.
 *
 * @param cpu_times Pointer to a cpu_times_t structure to populate.
 * @return 0 on success, -1 on failure.
 */
int read_cpu_times(cpu_times_t *out);

/**
 * @brief Reads the system's memory information from /proc/meminfo.
 * And, it populates the mem_info_t structure with the data read.
 *
 * @param mem_info Pointer to a mem_info_t structure to populate.
 * @return 0 on success, -1 on failure.
 */
int read_meminfo(mem_info_t *out);

/**
 * @brief Reads the system's uptime from /proc/uptime and formats it into days,
 * hours, and minutes.
 *
 * @param load_avg Pointer to a load_avg_t structure to populate with load
 * @return 0 on success, -1 on failure.
 */
int read_loadavg(load_avg_t *out);

/**
 * @brief Reads the system's uptime from /proc/uptime and returns it in seconds.
 *
 * @param uptime_sec Pointer to a double to store the uptime in seconds.
 * @return 0 on success, -1 on failure.
 */
int read_uptime(double *uptime_sec);

/**
 * @brief Counts the number of logged-in users by reading the utmp entries.
 *
 * @return The count of logged-in users.
 */
int count_logged_in_users(void); // utmp

/**
 * @brief Calculates the CPU usage percentages (for cpu_times_t structure) based
 * on the previous and current CPU times.
 *
 * @param prev Pointer to the previous CpuTimes structure.
 * @param now Pointer to the current CpuTimes structure.
 * @param percentages Pointer to a CpuPercentages structure to populate with
 *                    calculated percentages.
 */
void cpu_percent(const cpu_times_t *prev, const cpu_times_t *now,
                 cpu_percent_t *pct);

/**
 * @brief Formats the uptime in seconds into days, hours, and minutes.
 *
 * @param up The uptime in seconds.
 * @param o Pointer to an uptime_fmt_t structure to populate with formatted
 *          uptime.
 */
void fmt_uptime(double up, uptime_fmt_t *o);
