// src/core/process.h
#pragma once
#include <pwd.h>
#include <sys/types.h>

/**
 * Holds all relavant information for a single process.
 * The fields are chosen to match the columns in the 'top' command.
 */
typedef struct {
    pid_t pid;                       // Process ID
    char user[32];                   // User name of the process owner
    char priority[5];                // Process priority
    int nice;                        // Nice value
    unsigned long virt_mem;          // Virtual memory size in KiB
    unsigned long res_mem;           // Resident Set Size (RSS) in KiB
    unsigned long shr_mem;           // Shared memory size (SHR) in KiB
    char state;                      // Process state (e.g., R, S, Z, ...)
    double cpu_percent;              // CPU usage percentage
    double mem_percent;              // Memory usage percentage
    unsigned long long uptime_ticks; // Uptime in ticks
    char command[256];               // Command line of the process
} proc_info_t;

/**
 * Represents a dynamically-sized list of processes.
 */
typedef struct {
    proc_info_t *procs;
    size_t count;
    size_t capacity;
} proc_list_t;

void init_process_list(proc_list_t *list);
int collect_all_processes(proc_list_t *list,
                          unsigned long long system_mem_total);
void free_process_list(proc_list_t *list);
