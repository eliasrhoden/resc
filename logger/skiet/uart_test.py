
import serial 
import serial.tools.list_ports as tools
import os
import time



def main():
    print("RESC LOGGER")

    ports = tools.comports()

    if len(ports) > 1:
        raise Exception("MORE THAN ONE PORT")

    port = ports[0]
    print(port)

    with serial.Serial(port.name, 115200, timeout=1.0) as ser:


        ser.write('hepp'.encode('ascii'))
        ser.flush()
        resp = ser.read(4)
        print(resp)






if __name__ == '__main__':

    main()


