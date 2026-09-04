#include "stm32f446xx.h"
#include "stm32f4xx_hal.h"

#include <stdio.h>

UART_HandleTypeDef huart2;

int _write(int file, char *ptr, int len);

void Serial_Init(void);