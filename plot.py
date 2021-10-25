from os import name
import matplotlib.pyplot as plt
import pandas as pd

import soundfile as sf
import numpy as n

# senyal, fm = sf.read('p1_prova_mu.au')
# t = np.arange(0,len(senyal)) / fm
# plt.plot(t, senyal)

pwr = list(float(x.split()[0]) for x in open('ex4_power.txt').readlines())


zcr = list(float(x.split()[0]) for x in open('ex4_zcr.txt').readlines())


fig, axs = plt.subplots(2)
axs[0].plot(pwr)
axs[0].set(ylabel='Power')
axs[1].plot(zcr)
axs[1].set(ylabel='ZCR')

plt.show()