#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/dma.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>

// Static allocations
const uint8_t channel_length { 2 };
uint16_t channels[channel_length];
constexpr uint8_t chanlist[channel_length] { 1, 16 };
volatile bool flag;

uint8_t adc_get_num_channels()
{
  return channel_length;
}

void dma1_channel1_isr()
{
	flag = true;

	if (dma_get_interrupt_flag(DMA1, DMA_CHANNEL1, DMA_TCIF))
		dma_clear_interrupt_flags(DMA1, DMA_CHANNEL1, DMA_TCIF);
}

void dma_setup()
{
  nvic_enable_irq(NVIC_DMA1_CHANNEL1_IRQ);
  rcc_periph_clock_enable(RCC_DMA1);

  dma_channel_reset(DMA1, DMA_CHANNEL1);
  dma_set_priority(DMA1, DMA_CHANNEL1, DMA_CCR_PL_LOW);
  dma_set_memory_size(DMA1, DMA_CHANNEL1, DMA_CCR_MSIZE_16BIT);
  dma_set_peripheral_size(DMA1, DMA_CHANNEL1, DMA_CCR_PSIZE_16BIT);
  dma_enable_memory_increment_mode(DMA1, DMA_CHANNEL1);
  dma_enable_circular_mode(DMA1, DMA_CHANNEL1);
  dma_set_read_from_peripheral(DMA1, DMA_CHANNEL1);
  dma_set_peripheral_address(DMA1, DMA_CHANNEL1, (uint32_t)&ADC1_DR);
  dma_set_memory_address(DMA1, DMA_CHANNEL1, (uint32_t)channels);
  dma_set_number_of_data(DMA1, DMA_CHANNEL1, channel_length);
  dma_enable_transfer_complete_interrupt(DMA1, DMA_CHANNEL1);

  dma_enable_channel(DMA1, DMA_CHANNEL1);
}

void adc_setup()
{
  rcc_periph_clock_enable(RCC_GPIOA);
  rcc_periph_clock_enable(RCC_ADC1);

  adc_power_off(ADC1);

  dma_setup();

  rcc_periph_reset_pulse(RST_ADC1);
  rcc_set_adcpre(RCC_CFGR_ADCPRE_PCLK2_DIV6);

  adc_enable_scan_mode(ADC1);
  adc_set_single_conversion_mode(ADC1);
  adc_enable_external_trigger_regular(ADC1, ADC_CR2_EXTSEL_SWSTART);
  adc_set_right_aligned(ADC1);
  adc_set_sample_time_on_all_channels(ADC1, ADC_SMPR_SMP_239DOT5CYC);
  adc_set_regular_sequence(ADC1, channel_length, (uint8_t*)chanlist);

  adc_enable_dma(ADC1);
  adc_power_on(ADC1);

  adc_reset_calibration(ADC1);
  adc_calibrate(ADC1);

  adc_enable_temperature_sensor();
}

uint16_t* adc_sample() {
  flag = false;
  adc_start_conversion_regular(ADC1);
  while(!flag);
  return channels;
}
