

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

IU  = df['signal_2']
IV = df['signal_3']
IW = df['signal_4']

UU = df['signal_5']
UV = df['signal_6']
UW = df['signal_7']

deb0 = df['signal_8']
deb1 = df['signal_9']
deb2 = df['signal_10']
deb3 = df['signal_11']

plt.subplot(2,1,1)
plt.title('Encoder')
plt.plot(t_ms,theta)

plt.subplot(2,1,2)
plt.plot(t_ms,d_theta)

plt.figure()
plt.title('Phases')
plt.subplot(2,1,1)
plt.plot(t_ms,UU)
plt.plot(t_ms,UV)
plt.plot(t_ms,UW)

plt.subplot(2,1,2)
plt.plot(t_ms,IU)
plt.plot(t_ms,IV)
plt.plot(t_ms,IW)



plt.figure()
plt.title('DEBUG')
plt.plot(t_ms,deb0)
plt.plot(t_ms,deb1)
plt.plot(t_ms,deb2)
plt.plot(t_ms,deb3)

plt.show()
