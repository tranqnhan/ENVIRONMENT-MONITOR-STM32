
#include "stm32f446xx.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_i2c.h"

#include "scd41.h"

// SCD41 I2C address is 0x62
// HAL_I2C_Master_Transmit() and HAL_I2C_Master_Receive() expect the 7-bit address shifted left by one bit 
#define SCD41_I2C_ADDR_62 (0x62 << 1) 

void SCD41_Init(void) {
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
}



// Returns 1 on ready, 0 otherwise.
int SCD41_IsDeviceReady(void) {
    HAL_StatusTypeDef status;
    status = HAL_I2C_IsDeviceReady(&hi2c1, SCD41_I2C_ADDR_62, 3, 100);
    return status == HAL_OK;
}


HAL_StatusTypeDef SCD41_StartPeriodicMeasurement(void) {
    HAL_StatusTypeDef ret;

    uint8_t start_periodic_measurement_cmd[2] = {0x21, 0xB1};
    ret = HAL_I2C_Master_Transmit(
        &hi2c1, 
        SCD41_I2C_ADDR_62,
        start_periodic_measurement_cmd,
        2,
        1000); //Sending in Blocking mode
    

    return ret;
}

HAL_StatusTypeDef SCD41_StopPeriodicMeasurement(void) {
    HAL_StatusTypeDef ret;

    uint8_t stop_periodic_measurement_cmd[2] = {0x3F, 0x86};
    ret = HAL_I2C_Master_Transmit(
        &hi2c1, 
        SCD41_I2C_ADDR_62,
        stop_periodic_measurement_cmd,
        2,
        1000);
    

    return ret;
}


void SCD41_ReadMeasurements(Measurements* measurements) {
    uint8_t read_measurement_cmd[2] = {0xEC, 0x05};
    HAL_I2C_Master_Transmit(
        &hi2c1,
        SCD41_I2C_ADDR_62,
        read_measurement_cmd,
        2,
        1000
    );

    uint8_t measurement_respond[9] = {0};

    HAL_I2C_Master_Receive(
        &hi2c1,
        SCD41_I2C_ADDR_62,
        (uint8_t *)measurement_respond, 
        9,
        1000);

    uint16_t raw_carbon_ppm = (((uint16_t)measurement_respond[0]) << 8)| measurement_respond[1];
    uint16_t raw_temperature = (((uint16_t)measurement_respond[3]) << 8) | measurement_respond[4];
    uint16_t raw_humidity = (((uint16_t)measurement_respond[6]) << 8) | measurement_respond[7];
    

    // Conversion
    
    measurements->co2_ppm = raw_carbon_ppm;
    measurements->temperature = -45.0f + 175.0f * ((float)raw_temperature / 65535.0f);
    measurements->humidity = 100.0f * ((float)raw_humidity / 65535.0f);

}


// Returns 1 if ready, 0 if not
int SCD41_GetDataStatusReady(void) {
    int result = 0;

    uint8_t get_data_ready_status_cmd[2] = {0xe4, 0xb8};
    uint8_t data_ready_respond[3] = {0};

    HAL_I2C_Master_Transmit(
        &hi2c1, 
        SCD41_I2C_ADDR_62,
        get_data_ready_status_cmd,
        2,
        1000); 

    HAL_Delay(1);

    HAL_I2C_Master_Receive(
        &hi2c1 ,
        SCD41_I2C_ADDR_62,
        (uint8_t *)data_ready_respond, 
        3,
        1000);

    for (int i = 0; i < 3; ++i) {
        if (data_ready_respond[i] > 0) {
            result = 1;
            break;
        }
    }

    return result;
}


