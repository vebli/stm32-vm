#include "spi.h"
#include "graphics.h"

#define PIX_WIDTH 400
#define PIX_HEIGHT 240

uint8_t VCOM_bit = 1;

uint8_t reverse_bits(uint8_t byte) {
    byte = (byte >> 4) | (byte << 4);             
    byte = ((byte & 0xCC) >> 2) | ((byte & 0x33) << 2); 
    byte = ((byte & 0xAA) >> 1) | ((byte & 0x55) << 1); 
    return byte;
}

void lcd_draw_line(uint8_t *buffer, uint8_t line){
  uint8_t dummy_data[2]= {0x0, 0x0};
  uint8_t mode = 0x80 | (VCOM_bit << 6);
  uint8_t address = reverse_bits(line);

  HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
  HAL_SPI_Transmit(&hspi1, &mode, 1, HAL_MAX_DELAY);
  HAL_SPI_Transmit(&hspi1, &address, 1, HAL_MAX_DELAY);
  HAL_SPI_Transmit(&hspi1, buffer, PIX_WIDTH/8, HAL_MAX_DELAY);
  HAL_SPI_Transmit(&hspi1, dummy_data, sizeof(dummy_data), HAL_MAX_DELAY);
  HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);

  VCOM_bit ^= 1;
}

void lcd_clear(void){
  static uint8_t VCOM_bit = 1;
  uint8_t dummy_data[2]= {0x0, 0x0};
  uint8_t mode = 0x60 | (VCOM_bit << 6);

  HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
  HAL_SPI_Transmit(&hspi1, &mode, 1, HAL_MAX_DELAY);
  HAL_SPI_Transmit(&hspi1, dummy_data, sizeof(dummy_data), HAL_MAX_DELAY);
  HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);

  VCOM_bit ^= 1;
}

void lcd_draw(uint8_t *buffer){
  uint8_t mode = 0x80 | (VCOM_bit << 6);
  uint8_t address = 1; 
  uint8_t dummy_data[2]= {0x0, 0x0};

  HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
  HAL_SPI_Transmit(&hspi1, &mode, 1, HAL_MAX_DELAY);
  HAL_SPI_Transmit(&hspi1, &address, 1, HAL_MAX_DELAY);
  HAL_SPI_Transmit(&hspi1, buffer, PIX_WIDTH/8, HAL_MAX_DELAY);

  for(int i = 1; i < PIX_HEIGHT; i++){
      address = reverse_bits(i+1);
      HAL_SPI_Transmit(&hspi1, &dummy_data[0], sizeof(uint8_t), HAL_MAX_DELAY);
      HAL_SPI_Transmit(&hspi1, &address, 1, HAL_MAX_DELAY);
      HAL_SPI_Transmit(&hspi1, buffer + i*50, PIX_WIDTH/8, HAL_MAX_DELAY);
  }
  HAL_SPI_Transmit(&hspi1, dummy_data, sizeof(dummy_data), HAL_MAX_DELAY);
  HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
  VCOM_bit ^= 1;
}
