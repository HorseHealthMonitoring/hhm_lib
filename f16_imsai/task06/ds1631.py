import i2cDevice
import math
from time import sleep

startCon = chr(0x51)
stopCon = chr(0x22)
ReadTemp = chr(0xAA)
AccessTH = chr(0xA1)
AccessTL = chr(0xA2)
AccessCon = chr(0xAC)
SoftPOR = chr(0x54)

class ds1631:
    INIT_TH = 15.0
    INIT_TL = 10.0
    INIT_TEMP = -60.0
    INIT_CONF = 0x8C

    def __init__(self, i2cDevice):
        self.i2c = i2cDevice
        self.configReg = ds1631.INIT_CONF
        self.th = ds1631.INIT_TH
        self.tl = ds1631.INIT_TL
        self.tempC = ds1631.INIT_TEMP
        self.i2c.setAddress(0x90)
        # self.softwareReset()

    def softwareReset(self):
        while (self.isNonVolatileMemoryBusy()):
            sleep(0.001)
        self.i2c.open()
        self.i2c.write([SoftPOR])
        self.i2c.close()
        self.configReg = ds1631.INIT_CONF
        self.th = ds1631.INIT_TH
        self.tl = ds1631.INIT_TL
        self.tempC = ds1631.INIT_TEMP
    
    def getConfig(self):
        self.i2c.open()
        self.i2c.write([AccessCon])
        self.i2c.open()
        self.configReg = self.i2c.read(1)[0]
        print self.configReg
        self.i2c.close()
        return chr(self.configReg)
        
    def setConfig(self,configData):
        while (self.isNonVolatileMemoryBusy()):
            sleep(0.001)
        self.configReg = configData
        self.i2c.open()
        self.i2c.write([AccessCon, chr(configData)])
        self.i2c.close()
        
        
    def getTempC(self):
        while (self.isNonVolatileMemoryBusy()):
            sleep(0.001)
        self.i2c.open()
        self.i2c.write([ReadTemp])
        self.i2c.open()
        tempBuf = self.i2c.read(2)
        tempRegValue = (tempBuf[0] << 8) + tempBuf[1]
        self.tempC = self.__convertDS1631TempToFloat(tempRegValue)
        return self.tempC
        
    def getTempF(self):
        self.getTempC()
        return (self.tempC * (9.0/5.0)) + 32.0
        
        
    # Returns the thermostat high value in Celsius.
    def getTH(self):
        while (self.isNonVolatileMemoryBusy()):
            sleep(0.001)
        self.i2c.open()
        self.i2c.write([AccessTH])
        self.i2c.open()
        tempBuf = self.i2c.read(2)
        tempRegValue = ((tempBuf[0] << 8) & 0xFF00) + tempBuf[1]
        self.th = self.__convertDS1631TempToFloat(tempRegValue)
        self.i2c.close()
        return self.th
    
    # TH data must be in Celsius.
    def setTH(self,thData):
        while (self.isNonVolatileMemoryBusy()):
            sleep(0.001)
        self.th = thData
        thData = self.__convertDS1631FloatToTemp(thData)
        self.i2c.open()
        print '-------------' + str(thData >> 8) + '----------'
        print '-------------' + str(thData & 0x00FF) + '----------'
        self.i2c.write([AccessTH, chr((thData >> 8) & 0xFF), chr(thData & 0x00FF)])
        self.i2c.close()
    
    # Returns the thermostat low value in Celsius.
    def getTL(self):
        while (self.isNonVolatileMemoryBusy()):
            sleep(0.001)
        self.i2c.open()
        self.i2c.write([AccessTL])
        self.i2c.open()
        tempBuf = self.i2c.read(2)
        tempRegValue = ((tempBuf[0] << 8) & 0xFF00) + tempBuf[1]
        self.tl = self.__convertDS1631TempToFloat(tempRegValue)
        self.i2c.close()
        return self.tl
    
    # TL value must be in Celsius.
    def setTL(self, tlData):
        while (self.isNonVolatileMemoryBusy()):
            sleep(0.001)
        self.tl = tlData
        tlData = self.__convertDS1631FloatToTemp(tlData)
        self.i2c.open()
        self.i2c.write([AccessTL, chr((tlData >> 8) & 0xFF), chr(tlData & 0x00FF)])
        self.i2c.close()
        
    def startConvert(self):
        while (self.isNonVolatileMemoryBusy()):
            sleep(0.001)
        self.i2c.open()
        self.i2c.write([startCon])
        self.i2c.close()

        
    def stopConvert(self):
        while (self.isNonVolatileMemoryBusy()):
            sleep(0.001)
        self.i2c.open()
        self.i2c.write([stopCon])
        self.i2c.close()
    
    # Resolution (res) should only be values from 9-12.
    def setResolution(self, res):
        if 9 <= res <= 12:
            resBits = (res - 9) << 2
            config = (self.configReg & 0xF3) | resBits
            self.setConfig(config)
        else:
            raise ValueError("resolution must be between 9 and 12")
    
    # Should pass either True for active high or False for active low.
    # See DS1631 datasheet, pages 10-11.
    def setPolarity(self, pol):
        while (self.isNonVolatileMemoryBusy()):
            sleep(0.001)
        if not pol:
            config = self.configReg & 0xFD
            self.setConfig(config)
        else:
            config = self.configReg | 0x02
            self.setConfig(config)
    
    # True for one shot enabled, False for continuous mode.
    def setOneShot(self, oneshot):
        while (self.isNonVolatileMemoryBusy()):
            sleep(0.001)
        if not oneshot:
            config = self.configReg & 0xFE
            self.setConfig(config)
        else:
            config = self.configReg | 0x01
            self.setConfig(config)
    
    # Returns True for flag set, False otherwise.
    def getTHFlag(self):
        self.getConfig()  # make sure we have latest saved
        return True if (self.configReg & 0x40) > 0 else False
        
    # Returns True for flag set, False otherwise.
    def getTLFlag(self):
        self.getConfig()  # make sure we have latest saved
        return True if (self.configReg & 0x20) > 0 else False
    
    def clearTHFlag(self):
        config = self.configReg & 0b10111111  # sets THF = 0
        self.setConfig(config)
        
    def clearTLFlag(self):
        config = self.configReg & 0b11011111  # sets TLF = 0
        self.setConfig(config) 
    
    # Returns True if conversion complete, false otherwise.
    def isConversionDone(self):
        self.getConfig()
        return True if (self.configReg & 0x80) > 0 else False
        
    # Returns True if write is in progress, false otherwise.
    def isNonVolatileMemoryBusy(self):
        self.getConfig()
        return True if (self.configReg & 0x10) > 0 else False
            
    def __convertDS1631TempToFloat(self, word):
        temp = 0.0
        isNeg = False
        if (self.getSignSixteenBit(word) < 0):
            word = (~word) + 1
            isNeg = True
        temp += (word >> 8) & 0x00FF
        word = word & 0x00FF
        for i in range(8):
            bit = (word >> (7 - i)) & 0x0001
            temp += float(bit) * (math.pow(2, (-1 * (i + 1))))
        return temp if not isNeg else temp * -1

    def __convertDS1631FloatToTemp(self, temp):
        isNeg = False
        if (temp < 0.0):
            isNeg = True
        temp = math.fabs(temp)
        top_eight_bits = int(math.floor(temp))
        after_decimal = temp - top_eight_bits

        bottom_eight_bits = 0
        after_decimal = math.fabs(after_decimal)
        for i in range(8):
            pow = (-1 - i)
            check = math.pow(2, pow)
            if (after_decimal >= 0.0):
                if (after_decimal >= check):
                    bottom_eight_bits += int((math.pow(2, 7 - i)))
                    after_decimal -= check
        if isNeg:
            return ~(((top_eight_bits << 8)) + (bottom_eight_bits & 0x00FF)) + 1
        else:
            return ((top_eight_bits << 8)) + (bottom_eight_bits & 0x00FF)

    def getSignEightBit(self, n):
        return n if n <= 127 else -1 * n

    def getSignSixteenBit(self, n):
        return n if n <= 32767 else -1 * n
