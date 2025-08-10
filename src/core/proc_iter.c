// src/core/proc_iter.c
#include "system.h"
#include <ctype.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>

/**
 * Scan the /proc filesystem to count the number of tasks in various states.
 *
 * @param out Pointer to a TaskCounts structure to fill with the counts.
 * @return 0 on success, -1 on error (e.g., if /proc cannot be opened).
 */
int scan_task_states(TaskCounts *out) {
    memset(out, 0, sizeof(*out));

    DIR *d = opendir("/proc");
    if (!d) {
        return -1;
    }

    struct dirent *e;

    // Iterate through the entries in the /proc directory
    while ((e = readdir(d)) != NULL) {
        if (!isdigit((unsigned char)e->d_name[0])) {
            // Skip non-numeric directories (not a process)
            continue;
        }

        char path[256] = {0};
        snprintf(path, sizeof(path), "/proc/%s/stat", e->d_name);
        FILE *f = fopen(path, "re");
        if (!f) {
            continue;
        }

        // /proc/[pid]/stat format: "pid (comm) state ..."
        int pid;
        char comm[256] = {0};
        char state = '?';
        if (fscanf(f, "%d (%255[^)]) %c", &pid, comm, &state) == 3) {
            out->total++;
            switch (state) {
            case 'R':
                out->running++;
                break;
            case 'S':
                out->sleeping++;
                break;
            case 'D':
                // Uninterruptible sleep (usually IO)
                out->sleeping++;
                break;
            case 'T':
                out->stopped++;
                break;
            case 'Z':
                out->zombie++;
                break;
            default:
                // Unknown state, we can ignore it
                break;
            }
        }
        // Close the file descriptor
        fclose(f);
    }
    // Close the directory stream
    closedir(d);

    return 0;
}
