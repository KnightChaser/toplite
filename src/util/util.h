// src/util/util.h
#pragma once
#include <stdbool.h>
#include <stddef.h>

/**
 * @brief Reads the entire contents of a text file into a dynamically allocated
 * buffer safely.
 * @details It uses a dynamic buffer that grows as needed to accommodate the
 * file's content. The buffer is null-terminated.
 *
 * @param path The path to the file to read.
 * @param out Pointer to a char*  where the content will be stored.
 * @param len Pointer to a size_t where the length of the content will be
 * stored.
 * @return true on success, false on failure.
 */
bool read_text_file(const char *path, char **out, size_t *len);

/**
 * @brief Convert a string to an unsigned long long integer safely.
 * @details It skips leading whitespace. The input pointer 'p' is advanced past
 * the parsed number. This function does not handle negative numbers.
 *
 * @param p Pointer to the string to convert. The pointer will be updated
 * to point to the first character after the converted number.
 * @return The converted unsigned long long value.
 */
unsigned long long strtoull_safe(const char **p);

/**
 * @brief Get the system's memory page size
 *
 * @return The page size in bytes, or -1 on error.
 */
long sys_pagesize(void);

/**
 * @brief Get the system's clock ticks per second (USER_HZ)
 *
 * @return The number of clock ticks per second, or -1 on error.
 */
long sys_hz(void);
