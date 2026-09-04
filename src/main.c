
#include "stm32f446xx.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_i2c.h"

#include "serial.h"
#include "scd41.h"

int main() {
    
    HAL_Init();
    HAL_Delay(1000);

    Serial_Init();
    printf("Initializing...\n");
    
    SCD41_Init();
    HAL_Delay(1000);


    if (SCD41_IsDeviceReady()) {
        printf("SCD41 device is ready.\n");
    } else {
        printf("SCD41 device is not ready.\n");

    }

    printf("Start periodic measurements.\n");
    fflush(stdout);

    SCD41_StartPeriodicMeasurement();
    HAL_Delay(1000);

    Measurements scd41_measurements;

    while (1) {
        
        if (SCD41_GetDataStatusReady()) {
            SCD41_ReadMeasurements(&scd41_measurements);
            printf("CO2: %d ppm | temperature: %.2f C | humidity: %.2f%% \n",
                scd41_measurements.co2_ppm,
                scd41_measurements.temperature,
                scd41_measurements.humidity
            );

        } else {
            printf("Data is not ready.\n");
        }
        fflush(stdout);

        HAL_Delay(5000);
        
    }


    return 0;
}


void SysTick_Handler(void) {
    HAL_IncTick();
}
