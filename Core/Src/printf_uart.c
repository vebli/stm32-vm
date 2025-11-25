#include "printf_uart.h"
#include "usart.h"

int _write(int file, char *ptr, int len) {
    HAL_UART_Transmit(&huart1, (uint8_t *) ptr, len, HAL_MAX_DELAY);
    // HAL_UART_Transmit_IT(&huart1, (uint8_t *) ptr, len);
    return len;
}
