// src/ui/input.c
#include "input.h"
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

static struct termios orig_termios;

void disable_raw_mode(void) {
    // Restore the terminal attributes to their original state
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void enable_raw_mode(void) {
    // Get the current terminal attributes
    tcgetattr(STDIN_FILENO, &orig_termios);
    // Register disable_raw_mode to be called on program exit
    atexit(disable_raw_mode);

    struct termios raw = orig_termios;

    // Turn off canonical mode (line buffering) and echo
    raw.c_lflag &= ~(ICANON | ECHO);
    // Apply the new attributes immediately
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}
