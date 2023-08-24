#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/rcc.h>

#include "adc.h"
#include "usb.h"

// Set of ADC channels that are scanned
uint8_t channel_array[] = { 1 };

void adc_sample()
{
  uint16_t samples[32];
  for (int i = 0; i < 32; i++) {
    adc_start_conversion_direct(ADC1);
    while(!adc_eoc(ADC1));

    samples[i] = adc_read_regular(ADC1);
  }
  usbd_ep_write_packet(usbd_dev, 0x82, &samples, sizeof(uint16_t) * 32);
}

void adc_setup()
{
  rcc_periph_clock_enable(RCC_ADC1);

  adc_power_off(ADC1);
  adc_set_right_aligned(ADC1);
  adc_set_sample_time_on_all_channels(ADC1, ADC_SMPR_SMP_71DOT5CYC);
  adc_set_single_conversion_mode(ADC1);
  adc_power_on(ADC1);

  // Wait for ADC to start
  for (int i = 0; i < 800000; i++)
    __asm__("nop");

  // Calibrate
  adc_calibrate(ADC1);
  while (adc_is_calibrating(ADC1));

  adc_set_regular_sequence(ADC1, 1, channel_array);
}
