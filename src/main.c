
#include "stm32f446xx.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_i2c.h"

#include <stdio.h>
#include "serial.h"

// SCD41 I2C address is 0x62
// HAL_I2C_Master_Transmit() and HAL_I2C_Master_Receive() expect the 7-bit address shifted left by one bit 
#define SCD41_I2C_ADDR_62 (0x62 << 1) 

#define SCD4X_START_PERIODIC_MEASUREMENT_CMD_ID 0x21b1
#define SCD4X_READ_MEASUREMENT_RAW_CMD_ID       0xec05
#define SCD4X_GET_DATA_READY_STATUS_RAW_CMD_ID  0xe4b8
    
I2C_HandleTypeDef hi2c1;


void I2C_Init() {
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_I2C1_CLK_ENABLE();


    // Configuring GPIO for I2C

    GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;

    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);


    // Configure I2C hardware
    hi2c1.Instance = I2C1;
    hi2c1.Init.ClockSpeed = 100000;
    hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    
    HAL_I2C_Init(&hi2c1);
}


void LED_Init() {
    __HAL_RCC_GPIOA_CLK_ENABLE();
    
    GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;    
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;

    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

// Returns 1 on ready, 0 otherwise.
int IsDeviceReady() {
    HAL_StatusTypeDef status;

    status = HAL_I2C_IsDeviceReady(&hi2c1, SCD41_I2C_ADDR_62, 3, 100);

    int result;

    if (status == HAL_OK)
    {
        for (int i = 0; i < 3; ++i) {
            HAL_Delay(200);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
            HAL_Delay(200);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
        }
        result = 1;
    }
    else
    {
        for (int i = 0; i < 2; ++i) {
            HAL_Delay(2000);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
            HAL_Delay(1000);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
        }
        result = 0;
    }

    return result;
}



void SysTick_Handler(void) {
    HAL_IncTick();
}

int main() {
    HAL_Init();

    LED_Init();
    SerialInit();
    
    I2C_Init();


    //int result = IsDeviceReady();

    int count = 0;
    while(1) {
        printf("Hello %d\n", count++);
        HAL_Delay(1000);
        
    }




    return 0;
}