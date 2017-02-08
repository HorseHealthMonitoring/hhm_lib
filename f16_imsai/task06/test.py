from ds1631Serial import ds1631Serial
from i2cDeviceSerial import i2cDeviceSerial
import serial
from time import sleep
import math
# #
ds = ds1631Serial('COM4')
# x = ds.getConfig()
# print x




# s = serial.Serial()
# s.port='COM4'
# sleep(1)
# s.setRTS(False)
# s.setDTR(True)
# s.baudrate = 230400
# s.open()
# # s.open()
# # s.close()
#
# for st in "S90ACS91":
# 	print st
# 	s.write(st)
#
# s.write("0")
# s.write("1")
# sleep(5)
# print s.read(1)
# print s.read(1)
# s.close()
#
#print x.getTempC()