
import serial 
import serial.tools.list_ports as tools
import os
import time
import pandas as pd
import struct
import datetime
import matplotlib.pyplot as plt

def did_we_reviece_token(port_name,token):
    port_ok = False
    with serial.Serial(port_name, 115200, timeout=1) as ser:
        x = ser.read(len(token))
        #print(x)
        try:
            port_ok = x.decode() == token
        except:
            pass
    return port_ok

def start_logging(com_port):
    with serial.Serial(com_port, 115200, timeout=1.0) as ser:
        ser.write('hepp'.encode('ascii'))
        ser.flush()
        nr_floats = ser.read(4)
        #nr_floats = int.from_bytes(nr_floats,'big')
        nr_floats = struct.unpack('I',nr_floats)[0]
        if nr_floats > 30:
            raise Exception("EYYY " + str(nr_floats) + " thats alot of floats m8, us sure?")
        print(nr_floats)
        print("LOGGING STARTED!")
        frmt = 'I' + 'f'*nr_floats

        nr_bytes = 4*(nr_floats + 1) # + 1 time vecotr uint32

        logging_active = True 

        data = []
        logg_i = 0
        while logging_active:

            try:
                row = ser.read(nr_bytes)
                row = struct.unpack(frmt,row)
                #print(row)
                data.append(row)
            except struct.error as e:

                print("Connection lost")
                logging_active = False
            except KeyboardInterrupt:
                print("user aborted")
                logging_active = False

            logg_i += 1

        columns = [f'signal_{i}' for i in range(nr_floats)]
        columns.insert(0,'time_ms')
        df = pd.DataFrame(columns=columns,data = data)

        time_str = datetime.datetime.now().strftime('%y%m%d_%H%M')
        fname = 'logs/loggs_' + time_str + '.csv'
        print('Saving as: ' + fname)
        df.to_csv(fname,index=False)


def start_live_plot(com_port):

    time_history = 4
    time_history_ms = time_history*1000

    t_vec = []
    y_vecs = []

    with serial.Serial(com_port, 115200, timeout=1.0) as ser:
        ser.write('hepp'.encode('ascii'))
        ser.flush()
        nr_floats = ser.read(4)
        #nr_floats = int.from_bytes(nr_floats,'big')
        nr_floats = struct.unpack('I',nr_floats)[0]
        if nr_floats > 30:
            raise Exception("EYYY " + str(nr_floats) + " thats alot of floats m8, us sure?")
        print(nr_floats)

        frmt = 'I' + 'f'*nr_floats

        nr_bytes = 4*(nr_floats + 1) # + 1 time vecotr uint32

        legends = [f'Signal {i}' for i in range(nr_floats)]

        logging_active = True 

        N_HIST = 50

        while logging_active:
            plt.ioff()
            try:
                row = ser.read(nr_bytes)
                row = struct.unpack(frmt,row)
                #data.append(row)
                t_vec.append(row[0])
                y_vecs.append(row[1:])

                if len(t_vec) > N_HIST:
                    t_vec.pop(0)
                
                if len(y_vecs) > N_HIST:
                    y_vecs.pop(0)

                plt.clf()
                plt.plot(y_vecs)
                plt.pause(0.1)

            except struct.error as e:

                print("Connection lost")
                logging_active = False
            except KeyboardInterrupt:
                print("user aborted")
                logging_active = False






def main():
    print("RESC LOGGER")

    if not os.path.isdir('logs'):
        print("Logs dir not found, creating it")
        os.mkdir('logs')
    
    print("waiting for serial connection")
    found_token = False
    port = None
    while not found_token:

        ports = tools.comports()
        for p in ports:
            found_token = did_we_reviece_token(p.name,'glenn')
            if found_token:
                port = p.name
                break

        time.sleep(0.8)
        print("Waiting for token on serial ports...")
    print("LEETSGOO")
    start_logging(port)
    #start_live_plot(port)



if __name__ == '__main__':
    while True:

        main()
        print("Enter to start logging again")
        f = input()

