from time import sleep

import serial

ser = serial.Serial('/dev/tty.usbserial-1410',9600)

while True:

    read_serial = ser.readline()
    print ser.readline() # Read the newest output from the Arduino
    sleep(.1) # Delay for one tenth of a second
    ser.write('1')
