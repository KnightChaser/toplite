// src/util/util.h
#pragma once
#include <stdbool.h>
#include <stddef.h>

bool read_text_file(const char *path, char **out, size_t *len);
unsigned long long strtoull_safe(const char **p);
long sys_pagesize(void);
long sys_hz(void);
