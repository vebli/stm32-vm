#include "spi.h"
#include "adc.h"
#include "hardware.h"
#include "printf_uart.h"


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
  HAL_SPI_Transmit(&hspi1, buffer, LCD_PIX_WIDTH/8, HAL_MAX_DELAY);
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
  HAL_SPI_Transmit(&hspi1, buffer, LCD_PIX_WIDTH/8, HAL_MAX_DELAY);

  for(int i = 1; i < LCD_PIX_HEIGHT; i++){
      address = reverse_bits(i+1);
      HAL_SPI_Transmit(&hspi1, &dummy_data[0], sizeof(uint8_t), HAL_MAX_DELAY);
      HAL_SPI_Transmit(&hspi1, &address, 1, HAL_MAX_DELAY);
      HAL_SPI_Transmit(&hspi1, buffer + i*50, LCD_PIX_WIDTH/8, HAL_MAX_DELAY);
  }
  HAL_SPI_Transmit(&hspi1, dummy_data, sizeof(dummy_data), HAL_MAX_DELAY);
  HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
  VCOM_bit ^= 1;
}

#define ADC_MAX     4095
#define ADC_CENTER  2048
#define DEADZONE    400  

joystick_direction joystick_read(void){
   HAL_ADC_Start(&hadc3);
   HAL_ADC_PollForConversion(&hadc3, 10);
   uint32_t joy_x = HAL_ADC_GetValue(&hadc3);
   HAL_ADC_Stop(&hadc3);

   HAL_ADC_Start(&hadc2);
   HAL_ADC_PollForConversion(&hadc2, 10);
   uint32_t joy_y = HAL_ADC_GetValue(&hadc2);
   HAL_ADC_Start(&hadc2);

   // printf("x: %d\ty: %d\r\n", joy_x, joy_y);

   int horizontal = 0; 
   int vertical   = 0;
   if(joy_x > ADC_CENTER + DEADZONE)
       horizontal = -1;       
   else if(joy_x < ADC_CENTER - DEADZONE)
       horizontal = 1;     

   if(joy_y > ADC_CENTER + DEADZONE)
       vertical = -1;       
   else if(joy_y < ADC_CENTER - DEADZONE)
       vertical = 1;     

   // printf("h: %d\tv: %d\r\n", horizontal, vertical);

   if(vertical == 0 && horizontal == 0) return Z;
   if(vertical == 1 && horizontal == 0)  return N;
   if(vertical == 1 && horizontal == 1)  return NE;
   if(vertical == 0 && horizontal == 1)  return E;
   if(vertical == -1 && horizontal == 1) return SE;
   if(vertical == -1 && horizontal == 0) return S;
   if(vertical == -1 && horizontal == -1)return SW;
   if(vertical == 0 && horizontal == -1) return W;
   if(vertical == 1 && horizontal == -1) return NW;
}

int button_read(uint8_t num)
{
    uint8_t raw;

    if (num == 0) raw = HAL_GPIO_ReadPin(button_1_GPIO_Port, button_1_Pin);
    else raw = HAL_GPIO_ReadPin(button_2_GPIO_Port, button_2_Pin);

    static uint8_t last_state[2] = {1, 1};   

    uint8_t idx = (num == 0 ? 0 : 1);

    if (last_state[idx] == 1 && raw == 0) {
        last_state[idx] = raw;   
        return 1;               
    }

    last_state[idx] = raw;
    return 0;                  
}
