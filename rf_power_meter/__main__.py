import datetime
import serial

import numpy as np

import matplotlib.pyplot as plt
import matplotlib.animation as animation

device = "/dev/ttyACM0"

calibration = np.array([
    (0, -80),
    (2000, 0),
    (4096, 10)
]).T


def adc_to_dbm(adc):
    return np.interp(adc, calibration[0], calibration[1])


class PowerMeter:

    def __init__(self, port='/dev/ttyACM0', samp_length=128):
        self.samp_length = samp_length
        self.x = []
        self.y = []

        self.port = serial.Serial(port=port, baudrate=115200)

    def acquire(self):
        self.port.write('m'.encode('utf8'))
        self.port.flush()

        data = self.port.readline().decode('utf').strip()

        self.x.append(datetime.datetime.now())
        self.y.append(adc_to_dbm(int(data)))

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

        avg = sum(self.y[-10:]) / len(self.y[-10:])

        self.ax.text(0.1, 0.9, f'{avg:.2f} dBm',
                     fontsize=15, transform=self.ax.transAxes)

    def draw(self):
        fig, ax = plt.subplots()
        self.ax = ax
        ani = animation.FuncAnimation(fig, self.update, blit=False, interval=10)
        plt.show()


PowerMeter().draw()
