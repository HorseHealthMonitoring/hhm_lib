# This class serves as a wrapper for the pyserial class to
#   control I2C devices

# TODO: doxygen-ate this file
#       scan all serial ports to find the hardware via a key/ID string
#   
class i2cDevice:
    def __init__(self):
        pass
        
    def setPortName(self, portName):
        pass

    def setAddress(self, address):
        pass
        
    def open(self):
        pass
        
    def isOpen(self):
        pass
        
    def close(self):
        pass
        
    def getWriteAddress(self):
        pass

    def getReadAddress(self):
        pass
        
    def read(self, length):
        pass
    
    def write(self, buffer):
        pass


    
                
        
