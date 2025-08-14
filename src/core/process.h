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
    long nice;                       // Nice value
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

/**
 * @brief Initializes a process list (proc_list_t).
 * @details This function sets the initial state of the process list, preparing
 * it for adding processes.
 *
 * @param list Pointer to the proc_list_t to initialize.
 */
void init_process_list(proc_list_t *list);

/**
 * @brief Adds a single process to the process list (proc_list_t)
 * @details This function appends a new process information (proc_info_t) to the
 * list, resizing it if necessary.
 *
 * @param list Pointer to the proc_list_t where the process will be added.
 * @param process_info The proc_info_t structure containing the process
 * information to add.
 * @return 0 on success, -1 on error (e.g., if memory allocation fails).
 */
int collect_all_processes(proc_list_t *list,
                          unsigned long long system_mem_total);
/**
 * @brief Frees the memory allocated for a process list (proc_list_t).
 * @details This function releases all memory used by the process list,
 * including the array of processes.
 *
 * @param list Pointer to the proc_list_t to free.
 */
void free_process_list(proc_list_t *list);
