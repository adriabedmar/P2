from os import name
import matplotlib.pyplot as plt
import pandas as pd

import soundfile as sf
import numpy as np

senyal_original, fm1 = sf.read('pav_4301.wav')
senyal_silenciada, fm2 = sf.read('pav_4301_3.wav')
t1 = np.arange(0,len(senyal_original)) / fm1
t2 = np.arange(0,len(senyal_silenciada)) / fm2
#plt.plot(t, senyal_original, senyal_silenciada)

f, (ax1, ax2) = plt.subplots(1, 2)
ax1.plot(t1, senyal_original)
ax1.set_title('Original audio')
ax2.plot(t2, senyal_silenciada)
ax2.set_title('Silenced audio')

plt.show()

# pwr = list(float(x.split()[0]) for x in open('ex4_power.txt').readlines())


# zcr = list(float(x.split()[0]) for x in open('ex4_zcr.txt').readlines())


# fig, axs = plt.subplots(2)
# axs[0].plot(pwr)
# axs[0].set(ylabel='Power')
# axs[1].plot(zcr)
# axs[1].set(ylabel='ZCR')

# plt.show()