import unittest
import i2cDeviceSerial as device

ds1631Addr = 0x90
startConvertCommand = chr(0x51)
stopConvertCommand = chr(0x22)
ReadTemperatureCommand = chr(0xAA)
AccessThCommand = chr(0xA1)
AccessTlCommand = chr(0xA2)
AccessConfigCommand = chr(0xAC)
SoftwarePorCommand = chr(0x54)


class ds1631TestBench(unittest.TestCase):
    
    def setUp(self):
        i2cdev.setAddress(ds1631Addr)
    
    def test_measureTemperature(self):
        i2cdev.open()
        i2cdev.write([SoftwarePorCommand])  # reset config
        i2cdev.close()

        i2cdev.open()
        i2cdev.write([ReadTemperatureCommand])  # Send command to read the temperature
        i2cdev.close()

        i2cdev.open()
        readTemp = i2cdev.read(0x02)  # read the temperature
        i2cdev.close()
        print(readTemp)
        
        self.assertEqual(0xC4, readTemp[0])        


    def test_setTripPoints:
        tempHigh = chr(0x1B) #set high limit in 2C hex
        tempLow = chr(0x18) #set low limit in 2C hex

        # reset config
        i2cdev.open()
        i2cdev.write([SoftwarePorCommand])  
        i2cdev.close()

        #stop converting before modifying
        i2cdev.open()
        i2cdev.write([stopConvertCommand])
        i2cdev.close()

        #acces Th & send high point
        setTempHigh = [AccessThCommand, chr(0x00), tempHigh]
        i2cdev.open()
        i2cdev.write(setTempHigh) 
        i2cdev.close()

        #TODO: read temp high set point

        #access Tl & send low point
        setTempLow = [AccessTlCommand, chr(0x00), tempLow]
        i2cdev.open()
        i2cdev.write(setTempLow)
        i2cdev.close()

        #TODO: read low set point
        

    def test_thermostatFunction(self):
        print("Starting thermostat function test...")
        #read the current value of the config register
        i2cdev.open()
        i2cdev.write([AccessConfigCommand])
        i2cdev.open()
        configRegValue = i2cdev.read(0x01)
        print(configRegValue)
        i2cdev.close()

        # set THF and TLF flags to zero
        i2cdev.open()
        i2cdev.write([AccessConfigCommand, chr(configRegValue[0] & 0b10011101)])
        i2cdev.close()

        #read the current value of the config register
        i2cdev.open()
        i2cdev.write([AccessConfigCommand])
        i2cdev.open()
        configRegValue = i2cdev.read(0x01)
        print(configRegValue)
        i2cdev.close()
        
        #set the polarity to high
        i2cdev.open()
        i2cdev.write([AccessConfigCommand, chr(configRegValue[0] | 0b00000010)])
        i2cdev.close()

        #start convert
        i2cdev.open()       
        i2cdev.write([startConvertCommand])
        i2cdev.close()

        #read the current value of the config register
        i2cdev.open()
        i2cdev.write([AccessConfigCommand])
        i2cdev.open()
        configRegValue = i2cdev.read(0x01)
        i2cdev.close()

       
        #check until the T high flag is set
        while ( configRegValue[0] & 0x40):
            print("Waiting on temperature to exceed high trip point...")
            #Send command to read the temperature
            i2cdev.open()
            i2cdev.write([ReadTemperatureCommand])
            i2cdev.close()

            #read the temperature
            i2cdev.open()
            readTemp = i2cdev.read(0x02)
            print("Current temp:")
            print(readTemp)
            i2cdev.close()

            #read the current value of the config register
            i2cdev.open()
            i2cdev.write([AccessConfigCommand])
            i2cdev.open()
            configRegValue = i2cdev.read(0x01)
            i2cdev.close()
            
        print("Temp has exceeded high trip point.")
        self.assertFalse(configRegValue[0] & 0x40)

        #read the current value of the config register
        i2cdev.open()
        i2cdev.write([AccessConfigCommand])
        i2cdev.open()
        configRegValue = i2cdev.read(0x01)
        print(configRegValue)
        i2cdev.close()

        # set TLF flag to zero in case it may have been set before we tested the THF
        i2cdev.open()
        i2cdev.write([AccessConfigCommand, chr(configRegValue[0] & 0b11011101)])
        i2cdev.close()
            
        #check until the T low flag is set
        while ( configRegValue[0] & 0x20):
            print("Waiting on temperature to drop below low trip point...")
            #Send command to read the temperature
            i2cdev.open()
            i2cdev.write([ReadTemperatureCommand])
            i2cdev.close()

            #read the temperature
            i2cdev.open()
            readTemp = i2cdev.read(0x02)
            i2cdev.close()

            #read the current value of the config register
            i2cdev.open()
            i2cdev.write([AccessConfigCommand])
            i2cdev.open()
            configRegValue = i2cdev.read(0x01)
            i2cdev.close()
            
        print("Temp has gone below low trip point.")
        self.assertFalse(configRegValue[0] & 0x20)
        

    
        

if __name__ == "__main__":
    commName = input("Enter the name of the comm port you want to use with format COMX")
    i2cdev = device.i2cDeviceSerial(commName)
    unittest.main()
    i2cdev.close()
#demo proper operation of all DS1631 features, eg thermostat, alarm, etc
