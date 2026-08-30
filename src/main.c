
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

void I2CInit(void);
void LEDInit(void);
int IsI2CDeviceReady(void);

int main() {
    HAL_Init();

    SerialInit();
    I2CInit();

    if (!IsI2CDeviceReady()) {
        printf("SCD41 device is NOT ready.\n");
        //return 1;
    }

    int count = 0;
    while(1) {
        printf("Hello %d\n", count++);
        HAL_Delay(1000);
        
    }

    return 0;
}


void SysTick_Handler(void) {
    HAL_IncTick();
}



void I2CInit(void) {
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_I2C1_CLK_ENABLE();


    // Configuring GPIO for I2C

    GPIO_InitTypeDef GPIOInitStruct;

    GPIOInitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
    GPIOInitStruct.Mode = GPIO_MODE_AF_OD;
    GPIOInitStruct.Pull = GPIO_PULLUP;
    GPIOInitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIOInitStruct.Alternate = GPIO_AF4_I2C1;

    HAL_GPIO_Init(GPIOB, &GPIOInitStruct);


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
    HAL_Delay(200);
}


void LEDInit() {
    __HAL_RCC_GPIOA_CLK_ENABLE();
    
    GPIO_InitTypeDef GPIOInitStruct;

    GPIOInitStruct.Pin = GPIO_PIN_5;
    GPIOInitStruct.Mode = GPIO_MODE_OUTPUT_PP;    
    GPIOInitStruct.Pull = GPIO_PULLUP;
    GPIOInitStruct.Speed = GPIO_SPEED_HIGH;

    HAL_GPIO_Init(GPIOA, &GPIOInitStruct);

}

// Returns 1 on ready, 0 otherwise.
int IsI2CDeviceReady(void) {
    HAL_StatusTypeDef status;
    status = HAL_I2C_IsDeviceReady(&hi2c1, SCD41_I2C_ADDR_62, 3, 100);
    return status == HAL_OK;
}


