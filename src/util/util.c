// src/util/util.c
#include "util.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

bool read_text_file(const char *path, // [in]
                    char **out,       // [out]
                    size_t *len       // [out]
) {
    // Required out pointer doesn't exist
    if (!out) {
        return false;
    }

    FILE *f = fopen(path, "r");
    if (!f) {
        // Failed to open file
        return false;
    }

    size_t capacity = 4096, used = 0;
    char *buf = calloc(capacity, sizeof(char));
    if (!buf) {
        fclose(f);
        return false; // Memory allocation failed
    }

    while (true) {
        size_t bytes_to_read = capacity - used;
        if (bytes_to_read == 0) {
            // Need more space
            capacity *= 2;
            char *new_buf = realloc(buf, capacity);
            if (!new_buf) {
                free(buf);
                fclose(f);
                return false; // Memory allocation failed
            }
            buf = new_buf;
            bytes_to_read = capacity - used;
        }

        size_t bytes_read = fread(buf + used, 1, capacity - used, f);
        used += bytes_read;

        if (bytes_read < bytes_to_read) {
            // This indicates either EOF or a read error
            break;
        }
    }

    if (ferror(f)) {
        // An error occurred while reading
        free(buf);
        fclose(f);
        return false;
    }

    fclose(f);

    // Shrink to fit and null-terminate
    char *final_buf = realloc(buf, used + 1);
    if (!final_buf) {
        // Even though this final reallocation fails, we still return the
        // original buffer, even though it's a bit oversized.
        buf[used] = '\0';
        *out = buf;
    } else {
        final_buf[used] = '\0'; // Null-terminate the string
        *out = final_buf;       // Set the output pointer to the buffer
    }

    if (len) {
        *len = used;
    }

    return true; // Successfully read the file!
}

unsigned long long strtoull_safe(const char **p) {
    const char *s = *p;

    // Skip leading whitespace characters
    while (*s == ' ') {
        s++;
    }

    unsigned long long value = 0;
    while (*s >= '0' && *s <= '9') {
        // Ignore invalid characters
        value = value * 10 + (unsigned)(*s - '0');
        s++;
    }

    *p = s;

    return value;
}

long sys_pagesize(void) {
    long pagesize = sysconf(_SC_PAGESIZE);
    if (pagesize < 0) {
        // Handle error if needed
        return -1;
    }
    return pagesize;
}

long sys_hz(void) {
    long hz = sysconf(_SC_CLK_TCK);
    if (hz < 0) {
        // Handle error if needed
        return -1;
    }
    return hz;
}
