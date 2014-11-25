#ifndef TTY_H
#define TTY_H 1

#include <stdint.h>
#include <stddef.h>
#if !defined(__cplusplus)
#include <stdbool.h> /* C doesn't have booleans by default. */
#endif

/* Hardware text mode color constants. */
enum vga_color
{
	COLOR_BLACK = 0,
	COLOR_BLUE = 1,
	COLOR_GREEN = 2,
	COLOR_CYAN = 3,
	COLOR_RED = 4,
	COLOR_MAGENTA = 5,
	COLOR_BROWN = 6,
	COLOR_LIGHT_GREY = 7,
	COLOR_DARK_GREY = 8,
	COLOR_LIGHT_BLUE = 9,
	COLOR_LIGHT_GREEN = 10,
	COLOR_LIGHT_CYAN = 11,
	COLOR_LIGHT_RED = 12,
	COLOR_LIGHT_MAGENTA = 13,
	COLOR_LIGHT_BROWN = 14,
	COLOR_WHITE = 15,
};

void terminal_initialize(enum vga_color fg, enum vga_color bg);

void terminal_setcolor(uint8_t color);
 
uint8_t make_color(enum vga_color fg, enum vga_color bg);

void terminal_enable_cursor();

void terminal_disable_cursor();

void terminal_clear_screen();
 
void terminal_writestring(const char* data);

void terminal_scroll_down();

#endif
