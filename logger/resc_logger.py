
import serial 
import serial.tools.list_ports as tools
import os
import time
import pandas as pd
import struct
import datetime
import matplotlib.pyplot as plt

def did_we_reviece_token(ser,token):
    port_ok = False
    
    ser.write(ser.write('hepp'.encode('ascii')))
    ser.flush()
    x = ser.read(len(token))
    try:
        port_ok = x.decode() == token
    except:
        pass
    return port_ok

def start_logging(ser):

    nr_floats = ser.read(4)
    nr_floats = struct.unpack('I',nr_floats)[0]

    if nr_floats > 30:
        raise Exception("EYYY " + str(nr_floats) + " thats alot of floats m8, us sure?")
    print('Floats per timestamp',nr_floats)

    buffer_size = ser.read(4)
    buffer_size = struct.unpack('I',buffer_size)[0]

    if buffer_size > 1000 or buffer_size <= 0:
        raise Exception(f"Seems pretty wierd with a buffer size of {buffer_size}")

    print('Buffer size',buffer_size)

    print("LOGGING STARTED!")
    frmt = 'I' + 'f'*nr_floats

    frmt = frmt*buffer_size

    nr_bytes = len(frmt)*4
    print(nr_bytes)
    #nr_bytes = 16800
    print(nr_bytes)
    logging_active = True 

    data = []
    ser.timeout = 4.0
    while logging_active:

        try:
            row = ser.read(nr_bytes)
            row = struct.unpack(frmt,row)

            data.append(row)
        except struct.error as e:
            print('eee')
            print(e)
            print("Connection lost")
            logging_active = False
        except KeyboardInterrupt:
            print("user aborted")
            logging_active = False
        

    non_buff_data = []

    chunk_len = nr_floats + 1
    try:
        for row in data:
            for c in range(buffer_size-1):
                non_buff_data.append(row[c*chunk_len: (c+1)*chunk_len])
    except e: 
        print(e)

    columns = [f'signal_{i}' for i in range(nr_floats)]
    columns.insert(0,'time_ms')
    df = pd.DataFrame(columns=columns,data = non_buff_data)

    time_str = datetime.datetime.now().strftime('%y%m%d_%H%M')
    fname = 'logs/log_' + time_str + '.csv'
    print('Saving as: ' + fname)
    df.to_csv(fname,index=False)



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
            port = p.name
			#115200
            with serial.Serial(port, 115200, timeout=0.3) as ser:

                found_token = did_we_reviece_token(ser,'glenn') #'glenn
                if found_token:
                    start_logging(ser)
                    break
        if not found_token:
            time.sleep(0.5)
            print("Waiting for token on serial ports...")
    print("LEETSGOO")
    




if __name__ == '__main__':


    while True:

        main()
        print("Enter to start logging again")
        f = input()

