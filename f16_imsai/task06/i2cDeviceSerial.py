import serial
import i2cDevice
from time import sleep

ZERO_ASCII = ord("0")
CAPITAL_A_ASCII = ord("A")

class i2cDeviceSerial(i2cDevice.i2cDevice):
    start = "S"
    stop = "P"
    RTS = False
    DTR = True

    def __init__(self, PORT):
        i2cDevice.i2cDevice.__init__(self)
        self.port = PORT
        self.address = 0x00
        self.portName = PORT
        self.baudRate = 230400
        self.i2cOpen = False
        self.s = serial.Serial()
        self.configSerial(self.s)
        self.s.open()
    
    def setPortName (self, portName):
        self.portName = portName

    def setBaudRate (self, baudRate):
        self.baudrate = baudRate

    def open(self):
        # s = serial.Serial()
        # self.configSerial(s)
        # s.open()
        print i2cDeviceSerial.start
        self.s.write(i2cDeviceSerial.start)
        sleep(0.01)
        # s.close()
        self.i2cOpen = True

    def isOpen(self):
        return self.i2cOpen

    def close(self):
        if (self.isOpen()):
            # s = serial.Serial()
            # self.configSerial(s)
            # s.open()
            # self.configSerial(self.s)
            print i2cDeviceSerial.stop
            self.s.write(i2cDeviceSerial.stop)
            sleep(0.01)
            # s.close()
        else:
            print("can't close- not open")

    def setAddress(self, address):
        self.address = address
        
    def getWriteAddress(self):
        return self.address & 0xFE  # clear LSb

    def getReadAddress(self):
        return self.address | 0x01  # set LSb

    def read(self, length):
        results = []
        if (self.isOpen()):
            address = self.getReadAddress()
            addressSerialTop = chr(self.get_top_hex_char_from_byte(address))
            addressSerialBottom = chr(self.get_bottom_hex_char_from_byte(address))
            print addressSerialTop
            print addressSerialBottom
            # s = serial.Serial()
            # self.configSerial(s)
            # s.open()
            self.s.write(addressSerialTop)
            sleep(0.01)
            self.s.write(addressSerialBottom)
            sleep(0.01)
            lengthTop = chr(self.get_top_hex_char_from_byte(length))
            lengthBottom = chr(self.get_bottom_hex_char_from_byte(length))
            print lengthTop
            print lengthBottom

            self.s.write(lengthTop)
            sleep(0.01)
            self.s.write(lengthBottom)
            sleep(0.01)
            for i in range(length):
                print i
                hex = []

                # first char
                c = self.s.read(1)
                while(not c or not self.is_hex(c)):
                    c = self.s.read(1)
                hex.append(c)
                sleep(0.01)
                # second char
                c = self.s.read(1)
                while(not self.is_hex(c)):
                    c = self.s.read(1)
                hex.append(c)
                sleep(0.01)
                results.append(self.get_byte_from_hex_string(hex))
            # s.close()
        else:
            print('device must be open')
        return results
    
    def write(self, buffer):
        if (self.isOpen()):
            #address
            address = self.getWriteAddress()
            addressSerialTop = chr(self.get_top_hex_char_from_byte(address))
            addressSerialBottom = chr(self.get_bottom_hex_char_from_byte(address))
            # s = serial.Serial()
            # self.configSerial(s)
            # s.open()
            print addressSerialTop
            print addressSerialBottom
            self.s.write(addressSerialTop)
            sleep(0.01)
            self.s.write(addressSerialBottom)
            sleep(0.01)
            for i in buffer:
                u8_top = chr(self.get_top_hex_char_from_byte(ord(i)))
                u8_bottom = chr(self.get_bottom_hex_char_from_byte(ord(i)))
                self.s.write(u8_top)
                sleep(0.01)
                print u8_top
                self.s.write(u8_bottom)
                print u8_bottom
                sleep(0.01)
            # s.close()
        else:
            print ("can't write- not open")

    def get_top_hex_char_from_byte(self, byte):
        top = (byte >> 4) & 0x000F
        if top > 9:
            return top - 10 + CAPITAL_A_ASCII
        else:
            return top + ZERO_ASCII

    def get_bottom_hex_char_from_byte(self, byte):
        bottom = (byte) & 0x000F
        if bottom > 9:
            return bottom - 10 + CAPITAL_A_ASCII
        else:
            return bottom + ZERO_ASCII

    def get_byte_from_hex_string(self, hex):
        print 'hex: ' + hex[0] + hex[1]
        top = ord(hex[0])
        print 'top: ' + str(top)
        bottom = ord(hex[1])
        print 'bottom: ' + str(bottom)
        byte = 0
        if CAPITAL_A_ASCII <= top <= (CAPITAL_A_ASCII + 5):
            # A-F
            byte += (top - CAPITAL_A_ASCII + 10) << 4
        else:
            # 0-9
            byte += (top - ZERO_ASCII) << 4

        if CAPITAL_A_ASCII <= bottom <= (CAPITAL_A_ASCII + 5):
            # A-F
            byte += (bottom - CAPITAL_A_ASCII + 10)
        else:
            # 0-9
            byte += (bottom - ZERO_ASCII) 
        return byte

    def is_hex(self, c):
        print c
        return (CAPITAL_A_ASCII <= ord(c) <= CAPITAL_A_ASCII + 15) or (ZERO_ASCII <= ord(c) <= ZERO_ASCII + 9)


    def configSerial(self, s):
        s.port = self.portName
        s.baudrate = self.baudRate
        s.setRTS(i2cDeviceSerial.RTS)
        # s.setDTR(i2cDeviceSerial.DTR)
