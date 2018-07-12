import serial
import sys
import array
import time

maxtries = 15
tries = 0
sys.stdout.write("Opening serial port")
connected = False
while (tries<maxtries) and not connected:
    try:
        ser = serial.Serial('/dev/ttyACM0', 19200, timeout=20)  # open serial port
        connected = True
        sys.stdout.write('\n');
    except serial.SerialException:
        tries+=1
        sys.stdout.write('.');
        sys.stdout.flush();
        time.sleep(2)
        


#if(tries == maxtries-1)

fichier = sys.argv[1]
arr = array.array('B')
try:
    arr.fromfile(open(sys.argv[1], 'rb'), 1024*1024)
except EOFError:
    pass


#Erase
print "Erasing..."
data = bytearray([0x11,0x02,0x00,0x00,0x00,0x02])
ser.write(data)
ser.read()

#Send Data
print "Sending firmware..."
data=arr.tolist()
nb = 1
while(nb*0x400<=len(data)):
    part = data[(nb-1)*0x400:nb*0x400]
    chksum=(nb&0xFF)^(nb>>8 & 0xFF)^6
    for val in part:
        chksum^=val
    toSend = [0x11, 0x02, 0x04, nb&0xFF, nb>>8 & 0xFF] + part + [chksum]
    #print [hex(a) for a in toSend]
    nb+=1
    toSendArr = array.array('B')
    toSendArr.fromlist(toSend)
    ser.write(toSendArr)
    ser.read()
    sys.stdout.write('.')
    sys.stdout.flush()


#
#while (ser.inWaiting==0):
    
#print ser.read()
sys.stdout.write('\n')
ser.close()             # close port