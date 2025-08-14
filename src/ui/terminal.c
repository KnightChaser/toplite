// src/ui/terminal.c
#include "terminal.h"
#include <sys/ioctl.h>
#include <unistd.h>

/**
 * Gets the current size of the terminal window.
 *
 * @param term Pointer to a terminal_t structure where the size will be stored.
 */
void get_term_size(terminal_t *term) {
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1) {
        // If the ioctl call fails, fall back to a default size.
        // This might happen if output is redirected to a file.
        term->rows = 24;
        term->cols = 80;
    } else {
        term->rows = w.ws_row;
        term->cols = w.ws_col;
    }
}
