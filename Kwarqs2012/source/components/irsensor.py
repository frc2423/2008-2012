
try:
    import wpilib
except ImportError:
    import fake_wpilib as wpilib
    

class IRSensor(object):
    '''
        Encapsulates the functionality of the Sharp GP2D120 IR sensors 
        used to detect balls on the robot
    '''

    
    #by schematic, refers to sensor reporting ~14cm distance
    inditThresh = .9 
    #by schematic, refers to sensor reporting ~6cm distance
    #ballSet = 2
    
    def __init__(self, sensor_port, threshold):
        '''
            sensor_port     - analog channel number for sensor
            threshold       - Threshold used to determine when the ball 
                              is present. Value is usually 2.0, can be 
                              less depending on needs and positioning.
        '''
    
        self.irSensor = wpilib.AnalogChannel(sensor_port)
        self.ball_present_threshold = threshold
        
        self.state = False
            

    def IsBallPresent(self):
        '''
            Returns True if the sensor claims the ball is present
        '''
    
        voltage = self.GetVoltage()
        if voltage < IRSensor.inditThresh:
            self.state = False
        elif voltage > self.ball_present_threshold:
            self.state = True
        return self.state    
        
        
    def GetVoltage(self):
        '''Used for diagnostic purposes'''
        return self.irSensor.GetVoltage()
