#include "main.h"
#include "debug.c"

extern ADC_HandleTypeDef hadc1;

float battery_get_voltage()
{
    ADC_ChannelConfTypeDef sConfig = {0};

    // Configure the ADC channel for VBAT
    sConfig.Channel = ADC_CHANNEL_VBAT;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_12CYCLES_5; // Adjust sampling time as needed

    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
    {
        return -1.0f; // Return an error value
    }

    // Start ADC conversion
    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);

    // Get ADC value
    uint32_t raw_adc = HAL_ADC_GetValue(&hadc1);

    printf("rad ads:%li\n",raw_adc);

    // Stop ADC and clean up
    HAL_ADC_Stop(&hadc1);

    // Convert raw ADC value to voltage
    const float vref = 3.3f;          // Adjust if your VREF is different
    const uint32_t resolution = 4096; // 12-bit ADC resolution
    const float vbat_divider = 2.0f;  // Assuming a 1:2 divider for VBAT measurement

    float voltage = (raw_adc * vref / resolution) * vbat_divider;

    return voltage;
}