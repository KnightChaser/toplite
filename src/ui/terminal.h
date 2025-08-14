// src/ui/terminal.h
#pragma once

typedef struct {
    unsigned int rows;
    unsigned int cols;
} terminal_t;

void get_term_size(terminal_t *term);
