from i2cDeviceSerial import i2cDeviceSerial
from ds1631 import ds1631


class ds1631Serial(ds1631):
    def __init__(self, PORT):
        ds1631.__init__(self, i2cDeviceSerial(PORT))
        
    
    