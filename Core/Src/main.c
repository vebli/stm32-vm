/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "vm.h"
#include "printf_uart.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hardware.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t rx_byte;
const int cmd_buffer_size = 16;
char cmd_buffer[16];
const char help_msg[] = "Commands:\r\n\
run\r\n\
repl\r\n\
help\r\n\
";
const char prompt[]="\r\nSHELL > ";
int cmd_index = 0;
uint8_t send_prompt = 0;
int run_program_flag = 0;
int print_reg_flag = 0;
int step_flag = 0;
int print_program_flag = 0;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
/* USER CODE BEGIN PFP */
void clear_cmd_buffer(int from){
      for(int i = from; i < cmd_buffer_size; i++){
          cmd_buffer[i] = '\0';
      }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
    if(huart->Instance != USART1) return; 

    if(rx_byte == '\n' || rx_byte == '\r'){ //on enter terminals may send \r, \n, \r\n
        cmd_buffer[cmd_index] = '\0';
        if(strcmp(cmd_buffer, "run") == 0){
            const char msg1[] = "\r\nStarting program ...";
            HAL_UART_Transmit_IT(&huart1, (uint8_t*)msg1, strlen(msg1));
            run_program_flag = 1;
        }
        else if(strcmp(cmd_buffer, "reg") == 0 ){
            print_reg_flag = 1;
        }
        else if(strcmp(cmd_buffer, "step") == 0){
            step_flag = 1;
        }
        else if(strcmp(cmd_buffer, "instr") == 0){
            print_program_flag = 1;
        }
        else if (strcmp(cmd_buffer, "load") == 0) {
            printf("\tSend program size (max %d):\r\n", PROGRAM_SIZE_BYTES);

            // temporarily stop receiving interrupts
            HAL_UART_AbortReceive(&huart1);

            uint16_t size = 0;

            // receive 2-byte size (little endian)
            uint8_t size_buf[2];
            HAL_UART_Receive(&huart1, size_buf, 2, HAL_MAX_DELAY);
            size = size_buf[0] | (size_buf[1] << 8);

            while (size > PROGRAM_SIZE_BYTES) {
                printf("Program size %u too large. Send again:\r\n", size);
                HAL_UART_Receive(&huart1, size_buf, 2, HAL_MAX_DELAY);
                size = size_buf[0] | (size_buf[1] << 8);
            }

            printf("Waiting for %u bytes...\r\n", size);

            // receive program bytes
            HAL_UART_Receive(&huart1, (uint8_t *)program, size, HAL_MAX_DELAY);

            printf("Program loaded!\r\n");

            // resume shell input
            HAL_UART_Receive_IT(&huart1, &rx_byte, 1);

            clear_cmd_buffer(0);
            cmd_index = 0;
            send_prompt = 1;
            return; // avoid falling into the rest of the shell logic
        }
        else if(strcmp(cmd_buffer, "log") == 0){
            const char* msg = (vm_enable_logs) ? "\r\nDisabling Logs" : "\r\nEnabling Logs";
            HAL_UART_Transmit_IT(&huart1, (uint8_t*)msg, strlen(msg));
            vm_enable_logs ^= 1;
        }
        else if(strcmp(cmd_buffer, "help") == 0){
            HAL_UART_Transmit_IT(&huart1, (uint8_t*)help_msg, strlen(help_msg));
        }
        else{
            const char msg[] = "\r\nUnknown command";
            HAL_UART_Transmit_IT(&huart1, (uint8_t*)msg, strlen(msg));
        }
        clear_cmd_buffer(0);
        cmd_index = 0;
        send_prompt = 1;
    }
    else if(rx_byte == 0x7F || rx_byte == '\b'){ //backspace
        if(cmd_index > 0){
            cmd_buffer[--cmd_index] = '\0';
            HAL_UART_Transmit_IT(&huart1, (uint8_t*)"\b \b", 3);//move cursor back once, overwrite, move back again
        }
        else{
            cmd_index = 0;
        }
    }
    else{
        if(cmd_index < cmd_buffer_size){
            cmd_buffer[cmd_index++] = rx_byte;
        }
        else{
            clear_cmd_buffer(0);
            cmd_index = 0;
        }
        HAL_UART_Transmit_IT(&huart1, &rx_byte, 1);
    }
    HAL_UART_Receive_IT(&huart1, &rx_byte, 1);
}
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1) {
        if (send_prompt) {
            send_prompt = 0;
            HAL_UART_Transmit_IT(&huart1, (uint8_t*)prompt, strlen(prompt));
        }
    }
}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
      vm_init();
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* Configure the peripherals common clocks */
  PeriphCommonClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_SPI1_Init();
  MX_ADC3_Init();
  MX_ADC2_Init();
  /* USER CODE BEGIN 2 */
  HAL_UART_Receive_IT(&huart1, &rx_byte, 1);
  HAL_UART_Transmit_IT(&huart1, prompt, strlen(prompt));

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  lcd_clear();
  while (1)
  {
      // vm_draw();
      // HAL_Delay(1000);
      // lcd_clear();
      // HAL_Delay(1000);
      

      if(run_program_flag){
          while(vm_run_instruction()){}
      }
      else{
          if(print_reg_flag) {
              printf("\t");
              for (int i = 0; i < NUM_REGISTERS; i++){
                  printf("R%d: %05d\t", i, reg[i]);
              }
              printf("\r\n");
              print_reg_flag = 0;
          }
          if(step_flag) {
              vm_run_instruction();
              step_flag = 0;
          }
          if(print_program_flag){
              uint16_t word; 
              int i = 0;
              printf("\t");
              do{
                  word = program[pc + i];
                  printf("%04X ", word);
                  i++;
              } while(word != OP_HALT && i < 16);
              printf("\r\n");

              print_program_flag = 0;
          }
      }


    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE|RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 40;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enable MSI Auto calibration
  */
  HAL_RCCEx_EnableMSIPLLMode();
}

/**
  * @brief Peripherals Common Clock Configuration
  * @retval None
  */
void PeriphCommonClock_Config(void)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the peripherals clock
  */
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCCLKSOURCE_PLLSAI1;
  PeriphClkInit.PLLSAI1.PLLSAI1Source = RCC_PLLSOURCE_MSI;
  PeriphClkInit.PLLSAI1.PLLSAI1M = 1;
  PeriphClkInit.PLLSAI1.PLLSAI1N = 24;
  PeriphClkInit.PLLSAI1.PLLSAI1P = RCC_PLLP_DIV7;
  PeriphClkInit.PLLSAI1.PLLSAI1Q = RCC_PLLQ_DIV2;
  PeriphClkInit.PLLSAI1.PLLSAI1R = RCC_PLLR_DIV2;
  PeriphClkInit.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_ADC1CLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
