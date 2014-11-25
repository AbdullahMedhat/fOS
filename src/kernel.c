#include <stddef.h>
#if !defined(__cplusplus)
#include <stdbool.h> /* C doesn't have booleans by default. */
#endif

#include "tty/tty.h" 
 
#if defined(__cplusplus)
extern "C" /* Use C linkage for kernel_main. */
#endif
void kernel_main()
{
	terminal_initialize(COLOR_LIGHT_GREY, COLOR_BLACK);

	terminal_enable_cursor();

	terminal_writestring("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Phasellus vitae leo at dolor volutpat.");	
	terminal_scroll_down();
	terminal_scroll_down();
	terminal_scroll_down();
}
