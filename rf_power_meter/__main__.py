import datetime

import numpy as np

import matplotlib.pyplot as plt
import matplotlib.animation as animation

from .adc import ADC

calibration = np.array([
    (0, -80),
    (2000, 0),
    (4096, 10)
]).T


def adc_to_dbm(adc):
    return np.interp(adc, calibration[0], calibration[1])


class PowerMeter:

    def __init__(self, adc, samp_length=128):
        self.x = []
        self.y = []
        self.adc = adc
        self.samp_length = samp_length
        self.current = 0
        self.current_adc = 0

    def acquire(self):
        data = self.adc.acquire()

        self.current_adc = sum(data) / len(data)
        self.current = adc_to_dbm(self.current_adc)

        self.x.append(datetime.datetime.now())
        self.y.append(self.current)

        self.x = self.x[-self.samp_length:]
        self.y = self.y[-self.samp_length:]

        return zip(self.x, self.y)

    def update(self, t):
        self.acquire()
        self.ax.clear()

        self.ax.set_ylim(min(calibration[1]), max(calibration[1]))

        self.ax.set_ylabel('dBm')
        self.ax.set_xlabel('Time')
        self.ax.plot(self.x, self.y)
        self.ax.grid(visible=True)

        self.ax.set_title(f'{self.current:.2f} dBm, {self.adc.vbat():.2f}V, {self.adc.temp():.1f}C')

    def draw(self):
        fig, ax = plt.subplots()
        self.ax = ax
        ani = animation.FuncAnimation(fig, self.update, blit=False, interval=50)
        plt.show()


adc = ADC()
PowerMeter(adc).draw()
