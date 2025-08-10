// src/util.h
#pragma once
#include <stdbool.h>
#include <stddef.h>

bool read_text_file(const char *path, char **out, size_t *len);
char *trim(char *s);
unsigned long long strtoull_safe(const char **p);
long sys_pagesize(void);
long sys_hz(void);
