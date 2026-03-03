#pragma once

#include <stdbool.h>

#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

#if CONFIG_IDF_TARGET_ESP32
#define EXAMPLE_ADC1_CHAN0 ADC_CHANNEL_0
#else
#define EXAMPLE_ADC1_CHAN0 ADC_CHANNEL_0
#endif

#define EXAMPLE_ADC_ATTEN ADC_ATTEN_DB_12

extern int adc_raw[2][10];

bool example_adc_calibration_init(adc_unit_t unit,
                                  adc_channel_t channel,
                                  adc_atten_t atten,
                                  adc_cali_handle_t *out_handle);
