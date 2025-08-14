// src/ui/terminal.h
#pragma once

typedef struct {
    unsigned int rows;
    unsigned int cols;
} terminal_t;

/**
 * @brief Gets the current size of the terminal window.
 *
 * @param term Pointer to a terminal_t structure where the size will be stored.
 */
void get_term_size(terminal_t *term);
