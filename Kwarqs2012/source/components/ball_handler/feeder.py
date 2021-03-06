
try:
    import wpilib
except ImportError:
    import fake_wpilib as wpilib

from ..ir_sensor import IRSensor
from collections import deque


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
    
    def __init__(self, feederJag, low_ball_sensor, top_ball_sensor, mid_ball_sensor, enter_ball_sensor):
        
        # belt motor
        self.belt_motor = wpilib.Jaguar(feederJag)

        # sensors
        self.top_ball_sensor = IRSensor('IR-FTop', top_ball_sensor, 1.2)
        self.low_ball_sensor = IRSensor('IR-FLow', low_ball_sensor, 1.2)
        self.mid_ball_sensor = IRSensor('IR-FMid', mid_ball_sensor, 1.2)
        self.enter_ball_sensor = IRSensor('IR-FEnt', enter_ball_sensor, 1.2)
        
        self.direction = None
        self.direction_history = deque()
        self.last_direction = None
        self.has_low_ball = None
        self.has_top_ball = None
    
    def Feed(self):
        '''Pull balls into the robot and up to the shooter'''
        
        self.direction = Feeder.BELT_UP
        
    def Stop(self):
        '''Stops the feeder'''
        self.direction = Feeder.BELT_STOP
    
    def Expel(self):
        '''Pushes balls out of the feeder'''
        self.direction = Feeder.BELT_DOWN
    
    
    def GetDirection(self):
        '''Returns 1 if up, 0 if stopped, -1 if down'''
        if self.last_direction == Feeder.BELT_UP:
            return 1
        elif self.last_direction == Feeder.BELT_DOWN:
            return -1
        return 0

    
    def HasEnterBall(self):
        return self.enter_ball_sensor.IsBallPresent()
        
    def HasTopBall(self):
        '''Returns True if the top ball is present'''
        
        '''
        if self.has_top_ball == None:
            self.has_top_ball = self.top_ball_sensor.IsBallPresent()
        elif self.top_ball_sensor.IsBallPresent():
            self.has_top_ball = True
        elif self.low_ball_sensor.IsBallPresent() or not self.mid_ball_sensor.IsBallPresent():
            self.has_top_ball = False
        '''
        
        return self.top_ball_sensor.IsBallPresent()

        
    def HasMidBall(self):
        '''Returns True if a ball is in an intermediate state'''
        return self.mid_ball_sensor.IsBallPresent()
        
        
        
    def HasLowBall(self):
        '''Returns True if the bottom ball is present'''
        '''If LowBall sensor and TopBall sensor are false but MidBall sensor is true then has_low_ball needs to stay true'''
        
        '''
        if self.has_low_ball == None:
            self.has_low_ball = self.low_ball_sensor.IsBallPresent()
        elif self.low_ball_sensor.IsBallPresent():
            self.has_low_ball = True
        elif self.top_ball_sensor.IsBallPresent() or not self.mid_ball_sensor.IsBallPresent():
            self.has_low_ball = False
        '''
        
        return self.low_ball_sensor.IsBallPresent()

     
    def IsSet(self):
        '''Returns True if someone has called Feed/Stop/Expel since the
           last time that Update() was called'''
        return self.direction is not None
    
    
    def Update(self):
    
        self.top_ball_sensor.Update()
        self.mid_ball_sensor.Update()
        self.low_ball_sensor.Update()
        self.enter_ball_sensor.Update()
    
        direction = self.direction
    
        if direction is None:
            direction = Feeder.BELT_STOP
        
        self.belt_motor.Set(direction)
        
        # reset state
        self._calc_last_direction( direction )
        self.direction = None
        
        
    def _calc_last_direction(self, direction):
        '''Internal function: average belt direction tracking'''
        
        # average the last belt runs to determine whether the balls 
        # are probably traveling up or down
        num = len(self.direction_history) + 1
        
        if num > 2:
            self.direction_history.pop()
        
        self.direction_history.appendleft(direction)
        
        avg = sum(self.direction_history)/num
            
        if avg < 0.0:
            self.last_direction = Feeder.BELT_DOWN
        elif avg > 0.0:
            self.last_direction = Feeder.BELT_UP
        else:
            self.last_direction = Feeder.BELT_STOP

