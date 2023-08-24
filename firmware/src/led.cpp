#include <libopencm3/stm32/gpio.h>

void led_setup()
{
  gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);
  gpio_set(GPIOC, GPIO13);
}

void led_on()
{
  gpio_clear(GPIOC, GPIO13);
}

void led_off()
{
  gpio_set(GPIOC, GPIO13);
}
