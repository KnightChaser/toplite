// src/core/process.c
#include "process.h"
#include <ctype.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // For sysconf

/**
 * Initializes a process list (proc_list_t).
 * This function sets the initial state of the process list, preparing it for
 * adding processes.
 *
 * @param list Pointer to the proc_list_t to initialize.
 */
void init_process_list(proc_list_t *list) {
    list->procs = NULL;
    list->count = 0;
    list->capacity = 0;
}

/**
 * Adds a single process to the process list (proc_list_t)
 * This function appends a new process information (proc_info_t) to the list,
 * resizing it if necessary.
 *
 * @param list Pointer to the proc_list_t where the process will be added.
 * @param process_info The proc_info_t structure containing the process
 * information to add.
 * @return 0 on success, -1 on error (e.g., if memory allocation fails).
 */
static int add_process_to_list(proc_list_t *list,
                               const proc_info_t process_info) {
    if (list->count == list->capacity) {
        // Double the list if necessary
        size_t new_capacity = (list->capacity == 0) ? 64 : list->capacity * 2;
        proc_info_t *new_procs =
            realloc(list->procs, new_capacity * sizeof(proc_info_t));
        if (!new_procs) {
            perror("Failed to allocate memory for process list");
            return -1; // Memory allocation failed
        }
        list->procs = new_procs;
        list->capacity = new_capacity;
    }

    // Append the new process info to the list
    list->procs[list->count] = process_info;
    list->count++;
    return 0; // Success
}

/**
 * For a specific process ID, get the command line arguments.
 *
 * @param pid The process ID to query.
 * @param buffer Buffer to store the command line arguments.
 * @param size Size of the buffer.
 */
static void get_command_line(pid_t pid,    // [in]
                             char *buffer, // [out]
                             size_t size   // [in]
) {
    char path[256] = {0};
    snprintf(path, sizeof(path), "/proc/%d/cmdline", pid);
    FILE *f = fopen(path, "r");
    if (!f) {
        buffer[0] = '\0'; // No command line available
        return;
    }

    size_t bytes_read = fread(buffer, 1, size - 1, f);
    fclose(f);
    buffer[bytes_read] = '\0'; // Null-terminate the string

    // Replace null bytes with spaces for display
    for (size_t i = 0; i < bytes_read; i++) {
        if (buffer[i] == '\0') {
            buffer[i] = ' ';
        }
    }
}

/**
 * Collects all processes from the /proc filesystem and fills the ProcessList.
 * This function reads process information from /proc/[pid]/stat and
 * /proc/[pid]/status files, and calculates memory usage percentages based on
 * the total system memory.
 *
 * @param list Pointer to the proc_list_t where processes will be stored.
 * @param system_mem_total Total system memory in kilobytes (from
 * /proc/meminfo).
 * @return 0 on success, -1 on error (e.g., if /proc cannot be opened or
 *         if memory allocation fails).
 */
int collect_all_processes(proc_list_t *list,
                          unsigned long long system_mem_total) {
    DIR *d = opendir("/proc");
    if (!d) {
        perror("Could not open /proc");
        return -1;
    }

    list->count = 0; // Reset list for a fresh collection
    struct dirent *e;

    while ((e = readdir(d)) != NULL) {
        if (!isdigit((unsigned char)e->d_name[0])) {
            // Skip non-numeric directories (not a process (PID))
            continue;
        }

        pid_t pid = atoi(e->d_name);
        proc_info_t proc_info = {0};
        proc_info.pid = pid;

        char stat_path[256] = {0};
        snprintf(stat_path, sizeof(stat_path), "/proc/%d/stat", pid);
        FILE *stat_file = fopen(stat_path, "r");
        if (!stat_file)
            continue;

        // Parse /proc/[pid]/stat
        char comm[256] = {0};
        long priority_long;
        fscanf(stat_file,
               "%*d (%255[^)]) %c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u "
               "%llu %*u %ld %d", // Basic fields from stat
               comm, &proc_info.state, &proc_info.uptime_ticks, &priority_long,
               &proc_info.nice);
        fclose(stat_file);
        snprintf(proc_info.priority, sizeof(proc_info.priority), "%ld",
                 priority_long);

        // Fetch UID and memory info from /proc/[pid]/status
        char status_path[256] = {0};
        snprintf(status_path, sizeof(status_path), "/proc/%d/status", pid);
        FILE *status_file = fopen(status_path, "r");
        if (!status_file) {
            continue;
        }

        uid_t uid = 0;
        char line[256] = {0};
        while (fgets(line, sizeof(line), status_file)) {
            if (sscanf(line, "Uid:\t%u", &uid) == 1) {
                // Done with Uid, continue to get memory
            } else if (sscanf(line, "VmSize:\t%lu kB", &proc_info.virt_mem) ==
                       1) {
            } else if (sscanf(line, "VmRSS:\t%lu kB", &proc_info.res_mem) ==
                       1) {
            } else if (sscanf(line, "RssShmem:\t%lu kB", &proc_info.shr_mem) ==
                       1) {
            }
        }
        fclose(status_file);

        // Convert UID to username
        struct passwd *pw = getpwuid(uid);
        if (pw) {
            strncpy(proc_info.user, pw->pw_name, sizeof(proc_info.user) - 1);
        } else {
            // Failed? Use UID as a fallback
            snprintf(proc_info.user, sizeof(proc_info.user), "%u", uid);
        }

        // Get full command line
        get_command_line(pid, proc_info.command, sizeof(proc_info.command));
        if (proc_info.command[0] == '\0') {
            // Fallback to comm from stat if cmdline is empty (e.g. kernel
            // threads)
            strncpy(proc_info.command, comm, sizeof(proc_info.command) - 1);
        }

        // Calculate %MEM
        if (system_mem_total > 0) {
            proc_info.mem_percent =
                ((double)proc_info.res_mem / system_mem_total) * 100.0;
        }

        // TODO: Calculate %CPU (this is complex and will be done later)
        proc_info.cpu_percent = 0.0;

        if (add_process_to_list(list, proc_info) != 0) {
            break; // Stop if we can't add more processes
        }
    }
    closedir(d);
    return 0;
}
