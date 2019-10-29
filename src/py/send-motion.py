from time import sleep
import serial

##PORT = '/dev/tty.usbserial-1410'
PORT= '/dev/tty.usbmodem14101'

ser = serial.Serial(PORT,9600)

read_serial = ser.readline()
print "Reading:\n"
print ser.readline() # Read the newest output from the Arduino
print "----"
sleep(.1) # Delay for one tenth of a second
input ="C1|9999"
ser.write(input)
print "Sending:\n",input
print "----"


while 1:
    read_serial = ser.readline()
    print "Reading:\n"
    print ser.readline() # Read the newest output from the Arduino
    print "----"
    sleep(.1) # Delay for one tenth of a second
