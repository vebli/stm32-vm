#ifndef GRAPHICS_H
#define GRAPHICS_H
#include <stdint.h>

uint8_t reverse_bits(uint8_t byte);
void lcd_draw_line(uint8_t *buffer, uint8_t line); 
void lcd_draw(uint8_t *buffer); 
void lcd_clear(void);
 
#endif
