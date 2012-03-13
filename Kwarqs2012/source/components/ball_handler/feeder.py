
try:
    import wpilib
except ImportError:
    import fake_wpilib as wpilib

from components.ir_sensor import IRSensor


class Feeder(object):
    '''
        The feeder is the belt and sensors present on the bottom of the
        robot. This part of the robot moves balls from outside the robot up 
        into the chamber.
    '''

    # feeder belt motor values
    BELT_UP     = -1.0  # bring ball into the robot
    BELT_DOWN   = 1.0   # expel ball from robot
    BELT_STOP   = 0     # stop feeder belt
    
    def __init__(self, feederJag, low_ball_sensor, top_ball_sensor):
        
        # belt motor
        self.belt_motor = wpilib.Jaguar(feederJag)

        # sensors
        self.top_ball_sensor = IRSensor(top_ball_sensor, 1.2)
        self.low_ball_sensor = wpilib.DigitalInput( low_ball_sensor )
        
        self.direction = Feeder.BELT_STOP
        
    
    def Feed(self):
        '''Pull balls into the robot and up to the shooter'''
        
        self.direction = Feeder.BELT_UP
        
    def Stop(self):
        '''Stops the feeder'''
        self.direction = Feeder.BELT_STOP
    
    def Expel(self):
        '''Pushes balls out of the feeder'''
        self.direction = Feeder.BELT_DOWN
        
    def IsFull(self):
        '''Returns True if two balls are detected in the feeder'''
        
        if self.low_ball_sensor.Get() == True and self.top_ball_sensor.IsBallPresent() == True:
            return True
        return False
        
    def HasTopBall(self):
        '''Returns True if the top ball is present'''
    
        return self.top_ball_sensor.IsBallPresent()
        
    def Print(self):
            print("Feeder:")
            print("  IRSensor: %s; Is Ball Set: %s" % ( self.top_ball_sensor.GetVoltage(), self.top_ball_sensor.IsBallPresent() ))
            print("  Is full:  %s; HasTopBall: %s" % ( self.IsFull(), self.HasTopBall() ))

    def Update(self):
    
        self.belt_motor.Set(self.direction)
        self.direction = Feeder.BELT_STOP
        
    