import datetime

import numpy as np

import matplotlib.pyplot as plt
import matplotlib.animation as animation

from .adc import ADC

attenuation = 0

# White label directional coupler
attenuation = 31

# PCB attenuator
# attenuation = 13  # 10dB @ 960MHz, measured in NanoVNA Windows App
# attenuation = 20  # 20dB @ 960MHz, measured in NanoVNA Windows App
# attenuation = 33  # 30dB @ 960MHz, measured in NanoVNA Windows App

# Calibrated at 960 MHz with TinySA (square wave output?)
# calibration = np.array([
#     (1232, -5),
#     (1240, -13),
#     (1311, -17),
#     (1370, -20),
#     (1440, -23),
#     (1509, -26),
#     (1591, -29),
#     (1656, -31),
#     (1740, -34)
# ]).T

# From PlutoSDR using DD1US data
calibration = np.array([
    (745, -5),
    (852, -9),      # 70
    (960, -12.5),   # 60
    (1054, -16),    # 50
    (1173, -21),    # 40
    (1290, -24.5),  # 30
    (1400, -28),    # 20
    (1515, -32),
    (1620, -35),    # Drive level 0
]).T


def adc_to_dbm(adc):
    return np.interp(adc, calibration[0], calibration[1]) + attenuation


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
        self.y.append(self.current_adc)

        self.x = self.x[-self.samp_length:]
        self.y = self.y[-self.samp_length:]

        return zip(self.x, self.y)

    def update(self, t):
        self.acquire()
        self.ax.clear()

        self.ax.set_ylim(adc_to_dbm(max(calibration[0])), adc_to_dbm(min(calibration[1])))

        self.ax.set_ylabel('dBm')
        self.ax.set_xlabel('Time')
        self.ax.plot(self.x, [adc_to_dbm(y) for y in self.y])
        self.ax.grid(visible=True)

        self.ax.set_title(f'{self.current:.1f}dBm ({round(self.current_adc)}), -{attenuation}dB, {self.adc.vbat():.2f}V, {self.adc.temp():.1f}C')

    def draw(self):
        fig, ax = plt.subplots()
        self.ax = ax
        ani = animation.FuncAnimation(fig, self.update, blit=False, interval=50)
        plt.show()


adc = ADC()
PowerMeter(adc).draw()
