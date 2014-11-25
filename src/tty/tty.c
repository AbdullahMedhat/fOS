#include "tty.h"

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

uint16_t* vga_buffer;
uint16_t vga_idx_reg;
uint16_t vga_io_reg;
uint8_t vga_cursor_start;
 
size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
bool terminal_cursor_enabled;

uint16_t make_vgaentry(char c, uint8_t color)
{
	uint16_t c16 = c;
	uint16_t color16 = color;
	return c16 | color16 << 8;
}

void putentryat(char c, uint8_t color, size_t x, size_t y)
{
	if (vga_buffer == 0)
	{
		return;
	}

	const size_t index = y * VGA_WIDTH + x;
	vga_buffer[index] = make_vgaentry(c, color);
}

static __inline __attribute__((always_inline)) void outb(uint16_t port, uint8_t data) {
	__asm __volatile("outb %0, %w1" : : "a" (data), "d" (port));
}

static __inline __attribute__((always_inline)) uint8_t inb(uint16_t port) {
	uint8_t data;
	__asm __volatile("inb %w1, %0" : "=a" (data) : "d" (port));

	return data;
}

void putchar(char c)
{
	if (c != '\n')
	{
		putentryat(c, terminal_color, terminal_column, terminal_row);
		terminal_column++;
	}
	
	if (c == '\n' || terminal_column == VGA_WIDTH)
	{
		terminal_column = 0;
		if (++terminal_row == VGA_HEIGHT)
		{
			terminal_scroll_down();
		}
	}
}

void set_cursor_enable(bool enable)
{
	outb(vga_idx_reg, 0x0A);
	outb(vga_io_reg, vga_cursor_start | (enable == true ? 0x00 : 0x20));
	terminal_cursor_enabled = enable;
}

void set_cursor_size(uint8_t from, uint8_t to)
{
	outb(vga_idx_reg, 0x0A);
	uint8_t cursor_start = inb(vga_io_reg);
	outb(vga_idx_reg, 0x0B);
	uint8_t cursor_end = inb(vga_io_reg);

	cursor_start = (cursor_start & 0xC0) | from;
	cursor_end = (cursor_end & 0xE0) | to;

	outb(vga_idx_reg, 0x0A);
	outb(vga_io_reg, from);

	vga_cursor_start = from & 0x1F;

	outb(vga_idx_reg, 0x0B);
	outb(vga_io_reg, to);
}

void update_cursor()
{
	if (!terminal_cursor_enabled)
	{
		return;
	}

	const size_t position = terminal_row * VGA_WIDTH + terminal_column; 

	outb(vga_idx_reg, 0x0E);
	outb(vga_io_reg, (uint8_t)((position >> 8) & 0xFF));

	outb(vga_idx_reg, 0x0F);
	outb(vga_io_reg, (uint8_t)(position & 0xFF));
}

void terminal_initialize(enum vga_color fg, enum vga_color bg)
{
	vga_buffer = (uint16_t*)0xB8000;
	vga_idx_reg = *((uint16_t*)0x0463);
	vga_io_reg = vga_idx_reg + 1;

	outb(vga_idx_reg, 0x0A);
	uint8_t cursor_start_register = inb(vga_io_reg);

	vga_cursor_start = cursor_start_register & 0x1F;

	terminal_row = 0;
	terminal_column = 0;
	terminal_color = make_color(fg, bg);
	terminal_cursor_enabled = cursor_start_register & 0x20;

	set_cursor_enable(false);
	terminal_clear_screen();
}

void terminal_setcolor(uint8_t color)
{
	terminal_color = color;
}

uint8_t make_color(enum vga_color fg, enum vga_color bg)
{
	return fg | bg << 4;
}

void terminal_enable_cursor()
{
	if (terminal_cursor_enabled)
	{
		return;
	}
	
	set_cursor_size(14, 16);
	set_cursor_enable(true);
	terminal_cursor_enabled = true;
}

void terminal_disable_cursor()
{
	if (!terminal_cursor_enabled)
	{
		return;
	}
	
	set_cursor_enable(false);
	terminal_cursor_enabled = false;
}

void terminal_clear_screen()
{
	if (vga_buffer == 0)
	{
		return;
	}

	for (size_t y = 0; y < VGA_HEIGHT; y++)
	{
		for (size_t x = 0; x < VGA_WIDTH; x++)
		{
			const size_t index = y * VGA_WIDTH + x;
			vga_buffer[index] = make_vgaentry(' ', terminal_color);
		}
	}
}

void terminal_writestring(const char* data)
{	
	if (vga_buffer == 0)
	{
		return;
	}

	size_t i = 0;
	while (data[i] != 0)
	{
		putchar(data[i]);
		i++;
	}

	if (terminal_cursor_enabled)
	{
		update_cursor();
	}
}

void terminal_scroll_down()
{
	if (vga_buffer == 0 || terminal_row == 0)
	{
		return;
	}

	for (size_t y = 0; y < terminal_row; y++)
	{
		for (size_t x = 0; x < VGA_WIDTH; x++)
		{
			const size_t index = y * VGA_WIDTH + x;

			vga_buffer[index] = vga_buffer[index + VGA_WIDTH];
		}
	}

	size_t base = terminal_row * VGA_WIDTH;
	for (size_t x = 0; x < VGA_WIDTH; x++)
	{		
		vga_buffer[base + x] = make_vgaentry(' ', terminal_color);
	}

	terminal_row--;

	if (terminal_cursor_enabled)
	{
		update_cursor();
	}
}
