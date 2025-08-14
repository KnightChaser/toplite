// src/core/proc_iter.c
#include "system.h"
#include <ctype.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>

int scan_task_states(task_counts_t *out) {
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
        FILE *f = fopen(path, "r");
        if (!f) {
            continue;
        }

        // /proc/[pid]/stat format: "pid (comm) state ..."
        int pid;
        char comm[256] = {0};
        char state = '?';
        // NOTE: it will fail if a process name (comm) contains a right
        // parenthesis ). But this is rare in practice, and we can ignore it for
        // now.
        if (fscanf(f, "%d (%255[^)]) %c", &pid, comm, &state) == 3) {
            out->total++;
            switch (state) {
            case 'R': // Running
                out->running++;
                break;
            case 'S': // Interruptible sleep
            case 'D': // Uninterruptible disk sleep
            case 'I': // Idle kernel thread
                out->sleeping++;
                break;
            case 'T': // Stopped (by job control signal)
            case 't': // Tracing stop (stopped by debugger)
                out->stopped++;
                break;
            case 'Z': // Zombie (terminated but not reaped by parent)
                out->zombie++;
                break;
            default:
                // Unknown state (or very rare), we can ignore it
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
