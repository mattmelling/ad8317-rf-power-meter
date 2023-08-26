#ifndef ADC_H
#define ADC_H

#include <cstdint>

void adc_setup();
uint16_t* adc_sample();
uint8_t adc_get_num_channels();

#endif
