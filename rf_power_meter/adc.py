import usb.core
import usb.util


class ADC:

    def __init__(self, vendor=0x1209, product=0x0001):
        self._device = None
        self._vendor = vendor
        self._product = product

    @property
    def device(self):
        if self._device is not None:
            return self._device

        self._device = usb.core.find(idVendor=self._vendor,
                                     idProduct=self._product)

        if self._device is None:
            raise ValueError('Device not found')

        return self._device

    def acquire(self):
        self.device.ctrl_transfer(0x40, 0x12, 0, 0, None)
        data = self.device.read(0x82, 64, 100)
        return [(data[2 * i]) + (data[(2 * i) + 1] << 8)
                for i in range(int(len(data)/2))]
