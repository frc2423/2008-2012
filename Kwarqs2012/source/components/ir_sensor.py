
try:
    import wpilib
    import wpilib.SmartDashboard
except ImportError:
    import fake_wpilib as wpilib
    import fake_wpilib.SmartDashboard
    

class IRSensor(object):
    '''
        Encapsulates the functionality of the Sharp GP2D120 IR sensors 
        used to detect balls on the robot
    '''

    
    #by schematic, refers to sensor reporting ~14cm distance
    inditThresh = .9 
    #by schematic, refers to sensor reporting ~6cm distance
    #ballSet = 2
    
    def __init__(self, name, sensor_port, threshold):
        '''
            sensor_port     - analog channel number for sensor
            threshold       - Threshold used to determine when the ball 
                              is present. Value is usually 2.0, can be 
                              less depending on needs and positioning.
        '''
    
        self.irSensor = wpilib.AnalogChannel(sensor_port)
        self.ball_present_threshold = threshold
        self.name = name
        
        self.sd = wpilib.SmartDashboard.SmartDashboard.GetInstance()
        self.sd.PutDouble( self.name, 0.0 )
        self.sd.PutDouble( self.name + '-T', self.ball_present_threshold )
        
        self.state = False
            

    def IsBallPresent(self):
        '''
            Returns True if the sensor claims the ball is present
        '''
    
        voltage = self.GetVoltage()
        if voltage < IRSensor.inditThresh:
            self.state = False
        elif voltage > self.sd.GetDouble( self.name + '-T' ):
            self.state = True
        return self.state    
        
        
    def GetVoltage(self):
        '''Used for diagnostic purposes'''
        return self.irSensor.GetVoltage()

    def Update(self):
        self.sd.PutDouble( self.name, self.irSensor.GetVoltage() )