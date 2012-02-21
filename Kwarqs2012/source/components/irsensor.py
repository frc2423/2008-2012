'''
    Defines the functionality of the analog irsensors used to detect
'''
try:
    import wpilib
except ImportError:
    import fake_wpilib as wpilib
    
class IRSensor(object):
    
    '''
        values refers to distance based on schmatic of
        sharp GP2D120 and size of ball being ~10cm,
        should be verified imperically, sensors offset by
        5cm from ball
    '''
    #by schematic, refers to sensor reporting ~14cm distance
    inditThresh = .9 
    #by schematic, refers to sensor reporting ~6cm distance
    ballSet = 2 
    
    def __init__(self, irSensor):
        self.irSensor = wpilib.AnalogChannel(irSensor)
        self.state = False
            
    '''
        returns the current state of the ball, if a threshold is
        not passed then do not change the state
    '''
    def isBallSet(self):
        voltage = self.irSensor.GetVoltage()
        if voltage < IRSensor.inditThresh:
            self.state = False
        elif  voltage > IRSensor.ballSet:
            self.state = True
        return self.state    
        