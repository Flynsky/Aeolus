#include "main.h"
#include "debug.c"

extern ADC_HandleTypeDef hadc1;

float battery_get_voltage()
{
    HAL_ADC_Start(&hadc1);
     HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
     int raw_adc = HAL_ADC_GetValue(&hadc1);

    
}