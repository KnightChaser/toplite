// src/ui/input.h
#pragma once

/**
 * @brief Enables raw mode for the terminal.
 * @details Disables canonical mode (line buffering) and echoing of input
 *          characters. This function must be paired with disable_raw_mode().
 */
void enable_raw_mode(void);

/**
 * @brief Disables raw mode, restoring the terminal to its original state.
 * @details This should be called before the program exits to avoid leaving the
 *          user's terminal in a broken state.
 */
void disable_raw_mode(void);
