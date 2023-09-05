

import pandas as pd 
import matplotlib.pyplot as plt 
import os 
import sys


def find_latest_log(indx=-1):
    folder = 'logs'
    logs = os.listdir(folder)
    return folder + os.sep + logs[indx]

indx = -1

if len(sys.argv) > 1:
    indx = int(sys.argv[1])

log = find_latest_log(indx)
print(log)
df = pd.read_csv(log)


t_ms = df['time_ms']
t_ms -= t_ms[0]

theta = df['signal_0']
d_theta = df['signal_1']



plt.subplot(2,1,1)
plt.title('Encoder')
plt.plot(t_ms,theta)

plt.subplot(2,1,2)
plt.plot(t_ms,d_theta)

plt.show()
