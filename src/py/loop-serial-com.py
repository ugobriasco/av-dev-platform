from time import sleep
import serial

ser = serial.Serial('/dev/tty.usbserial-1410',9600)

print "Hello"

while 1:
    read_serial = ser.readline()
    print "Reading:\n"
    print ser.readline() # Read the newest output from the Arduino
    print "----"
    sleep(.1) # Delay for one tenth of a second
    ##input = 'C10101'
    ## ser.write(input.decode("hex"))
    ###print "Sending:\n",input.decode("hex")

    input ="C1:1099"
    ser.write(input)
    print "Sending:\n",input

    print "----"
