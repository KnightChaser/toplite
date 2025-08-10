// src/util.c
#include "util.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/**
 * Reads a text file and returns its content.
 *
 * @param path The path to the file to read.
 * @param out Pointer to a char pointer where the content will be stored.
 * @param len Pointer to a size_t where the length of the content will be
 * stored.
 * @return true on success, false on failure.
 */
bool read_text_file(const char *path, // [in]
                    char **out,       // [out]
                    size_t *len       // [out]
) {
    FILE *f = fopen(path, "r");
    if (!f) {
        // Failed to open file
        return false;
    }

    size_t capacity = 4096, used = 0;
    char *buf = calloc(capacity + 1, sizeof(char));
    if (!buf) {
        fclose(f);
        return false; // Memory allocation failed
    }

    while (true) {
        if (capacity - used == 0) {
            // Need more space
            capacity *= 2;
            char *new_buf = realloc(buf, capacity + 1);
            if (!new_buf) {
                free(buf);
                fclose(f);
                return false; // Memory allocation failed
            }
            buf = new_buf;
        }

        size_t bytes_read = fread(buf + used, 1, capacity - used, f);
        used += bytes_read;

        if (bytes_read == 0) {
            // End of file (or an error)
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

    // Null-terminate the string
    buf[used] = '\0';

    if (out) {
        *out = buf; // Set the output pointer to the buffer
    }
    if (len) {
        *len = used; // Set the length of the content
    }

    return out != NULL;
}

/**
 * Trim leading and trailing whitespace characters from a string.
 * This function modifies the input string in place.
 *
 * @param s The string to trim.
 * @return A pointer to the trimmed string.
 */
char *trim(char *s) {
    while (isspace((unsigned char)*s)) {
        s++;
    }

    // Handle all-whitespace strings and now there's only a null terminator
    if (*s == '\0') {
        return s;
    }

    // Trim trailing whitespaces
    char *e = s + strlen(s) - 1;
    while (e > s && isspace((unsigned char)*e)) {
        e--;
    }

    *(e + 1) = '\0'; // Null-terminate the trimmed string

    return s; // Return the trimmed string
}

/**
 * Convert a string to an unsigned long long integer safely.
 * This function skips leading whitespace and ignores invalid characters.
 *
 * @param p Pointer to the string to convert. The pointer will be updated
 * to point to the first character after the converted number.
 * @return The converted unsigned long long value.
 */
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

/**
 * Get the system page size.
 *
 * @return The page size in bytes, or -1 on error.
 */
long sys_pagesize(void) {
    long pagesize = sysconf(_SC_PAGESIZE);
    if (pagesize < 0) {
        // Handle error if needed
        return -1;
    }
    return pagesize;
}

/**
 * Get the system clock ticks per second (Hz).
 *
 * @return The number of clock ticks per second, or -1 on error.
 */
long sys_hz(void) {
    long hz = sysconf(_SC_CLK_TCK);
    if (hz < 0) {
        // Handle error if needed
        return -1;
    }
    return hz;
}
