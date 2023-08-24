#include <stdlib.h>

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>

#include "usb.h"
#include "led.h"
#include "adc.h"

usbd_device *usbd_dev;
uint8_t usbd_control_buffer[128];

enum usbd_request_return_codes usb_control_request(usbd_device *usbd_dev, struct usb_setup_data *req, uint8_t **buf,
		uint16_t *len, void (**complete)(usbd_device *usbd_dev, struct usb_setup_data *req))
{
 	(void)complete;
	(void)buf;
	(void)usbd_dev;

    if(req->bRequest == USB_CTRL_LED_OFF) {
      led_off();
      return USBD_REQ_HANDLED;
    }

    if(req->bRequest == USB_CTRL_LED_ON) {
      led_on();
      return USBD_REQ_HANDLED;
    }

    if(req->bRequest == USB_CTRL_SAMPLE) {
      adc_sample();
      return USBD_REQ_HANDLED;
    }

	return USBD_REQ_NOTSUPP;
}

void usb_data_rx_cb(usbd_device *usbd_dev, uint8_t ep)
{
	(void)ep;
	(void)usbd_dev;

	char buf[64];
	int len = usbd_ep_read_packet(usbd_dev, 0x01, buf, 64);

    if((int)buf[0] == 1) {
      led_on();
    }
    if((int)buf[0] == 0) {
      led_off();
    }
}

void usb_set_config(usbd_device *usbd_dev, uint16_t wValue)
{
	(void)wValue;
	(void)usbd_dev;

	usbd_ep_setup(usbd_dev, 0x01, USB_ENDPOINT_ATTR_BULK, 64, usb_data_rx_cb);
	usbd_ep_setup(usbd_dev, 0x82, USB_ENDPOINT_ATTR_BULK, 64, NULL);

	usbd_register_control_callback(usbd_dev,
                                   USB_REQ_TYPE_VENDOR | USB_REQ_TYPE_DEVICE,
                                   USB_REQ_TYPE_VENDOR | USB_REQ_TYPE_RECIPIENT,
                                   usb_control_request);
}

void usb_setup()
{
  rcc_periph_clock_enable(RCC_GPIOC);

  /* Setup GPIOC Pin 12 to pull up the D+ high, so autodect works with the
   * bootloader.  The circuit is active low. */
  gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_OPENDRAIN, GPIO12);
  gpio_clear(GPIOC, GPIO12);

  usbd_dev = usbd_init(&st_usbfs_v1_usb_driver, &dev, &config, usb_strings, 3, usbd_control_buffer, sizeof(usbd_control_buffer));
  usbd_register_set_config_callback(usbd_dev, usb_set_config);
}
