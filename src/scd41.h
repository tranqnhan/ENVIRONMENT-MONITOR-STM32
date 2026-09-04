
#include "stm32f446xx.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_i2c.h"


typedef struct {
    uint16_t co2_ppm;
    float temperature;
    float humidity;
} Measurements;


I2C_HandleTypeDef hi2c1;

void SCD41_Init(void);
int SCD41_IsDeviceReady(void);
HAL_StatusTypeDef SCD41_StartPeriodicMeasurement(void);
HAL_StatusTypeDef SCD41_StopPeriodicMeasurement(void);

void SCD41_ReadMeasurements(Measurements* measurements);

int SCD41_GetDataStatusReady(void);




