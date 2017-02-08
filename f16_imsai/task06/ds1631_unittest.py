#!/usr/bin/python

import unittest
import ds1631Serial as ds
import time


class testConfig(unittest.TestCase):
    
    def testSetConfig(self):
        getConfigByteOld = d.getConfig()
        d.setConfig(0xFF)
        setConfigByte = 0x0A
        d.setConfig(0x0A)
        time.sleep(1)
        getConfigByte = d.getConfig()
        getConfigByte = ord(getConfigByte)&0x0F
        self.assertEqual(setConfigByte, getConfigByte)


    def testSetResolution(self):

        d.setResolution(9)
        configByte = ord(d.getConfig())
        if ((~configByte) & 0x08) and ((~configByte) & 0x04):
            result = True
        else:
            result = False

        self.failUnless(result == True, "Res not set.")

        d.setResolution(10)
        configByte = ord(d.getConfig())
        if ((~configByte) & 0x08) and (configByte & 0x04):
            result = True
        else:
            result = False

        self.failUnless(result == True)

        d.setResolution(11)
        configByte = ord(d.getConfig())
        if (configByte & 0x08) and ((~configByte) & 0x04):
            result = True
        else:
            result = False

        self.failUnless(result == True)

        d.setResolution(12)
        configByte = ord(d.getConfig())

        if (configByte & 0x08) and (configByte & 0x04):
            result = True
        else:
            result = False

        self.failUnless(result == True)

    def testSetTH(self):
        d.setResolution(12)
        for i in range(0, 150):
            THOld = -25.0625 + (.0625 * i)

            d.setTH(THOld)
            THNew = d.getTH()
            self.assertEqual(THOld, THNew)



    def testSetTL(self):
        for i in range(0, 150):
            TLOld = -25.0625 + (.0625 * i)

            d.setTL(TLOld)
            TLNew = d.getTL()
            self.assertEqual(TLOld, TLNew)



    def testSetPolarity(self):
        d.setPolarity(False)
        d.setPolarity(True)
        configByte = ord(d.getConfig())
        if (configByte & 0x02):
            result = True
        else:
            result = False

        self.failUnless(result == True)

        d.setPolarity(False)
        configByte = ord(d.getConfig())
        if ((~configByte) & 0x02):
            result = True
        else:
            result = False

        self.failUnless(result == True)

    def testSetOneShot(self):
        d.setOneShot(False)
        d.setOneShot(True)
        configByte = ord(d.getConfig())
        if (configByte & 0x01):
            result = True
        else:
            result = False

        self.failUnless(result == True)

        d.setOneShot(False)
        configByte = ord(d.getConfig())
        if ((~configByte) & 0x01):
            result = True
        else:
            result = False

        self.failUnless(result == True)

    def testIsConversionDone(self):
        d.stopConvert()
        time.sleep(1)
        d.setOneShot(False)
        result = d.isConversionDone()
        self.failUnless(result == True)

        d.startConvert()
        result = d.isConversionDone()
        self.failUnless(result == False)

    def testIsNonVolatileMemoryBusy(self):
        time.sleep(.5)
        configByte = ord(d.getConfig())
        if (configByte & 0x02):
            result = False
        else:
            result = True
        self.failUnless(result == True)
        d.setPolarity(True)
        configByte = ord(d.getConfig())
        if (configByte & 0x02):
            result = True
        else:
            result = False
        self.failUnless(result == True)

    def testThermostatLowFunctions(self):
        d.startConvert()
        d.setTH(50.0)
        d.setTL(-30.0)
        d.clearTLFlag()
        d.clearTHFlag()
        result = d.getTLFlag()

        self.failUnless(result == False)

        result = d.getTHFlag()
        self.failUnless(result == False)

        currentTemp = d.getTempC()
        if (currentTemp > 25) or (currentTemp < 21):
            result = False
        else:
            result = True
        self.failUnless(result == True)

        for i in range(1, 5):
            d.setTL(currentTemp + (5.0 * i))
            time.sleep(1)
            result = d.getTLFlag()

            self.failUnless(result == True)
            d.setTL(currentTemp - (5.0 * i))
            d.clearTLFlag()
            result = d.getTLFlag()

            self.failUnless(result == False)

    def testThermostatHighFunctions(self):
        d.startConvert()
        d.setTH(50.0)
        d.setTL(-30.0)
        d.clearTLFlag()
        d.clearTHFlag()
        currentTemp = d.getTempC()
        if (currentTemp > 25) or (currentTemp < 21):
            result = False
        else:
            result = True
        self.failUnless(result == True)

        for i in range(1, 5):
            d.setTH(currentTemp - (5.0 * i))
            time.sleep(1)
            result = d.getTHFlag()

            self.failUnless(result == True)
            d.setTH(currentTemp + (5.0 * i))
            d.clearTHFlag()
            result = d.getTHFlag()

            self.failUnless(result == False)



if __name__ == "__main__":
    d = ds.ds1631Serial('COM4')
    unittest.main()
    d.close()
