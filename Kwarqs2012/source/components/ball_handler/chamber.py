

try:
    import wpilib
except ImportError:
    import fake_wpilib as wpilib

from ..ir_sensor import IRSensor
from collections import deque


class Chamber(object):
    '''
        The chamber is located on the top part of the robot. It has a belt
        and an IR sensor. 
        
        This part of the robot receives balls from the feeder. It holds balls
        until we're ready to shoot, and then moves balls into the shooter.
        
        NOTE: When feeding balls into the chamber, the feeder and the
        chamber belts must be running, otherwise balls will get stuck.
    '''
    
    # belt movement values.. 
    BELT_UP     = wpilib.Relay.kReverse
    BELT_DOWN   = wpilib.Relay.kForward
    BELT_OFF    = wpilib.Relay.kOff
    
    
    def __init__ ( self, chamberRelay, chambIRSens ):
    
        self.belt_motor = wpilib.Relay(chamberRelay)
        self.ball_sensor = IRSensor(chambIRSens, 2.0)
        
        self.direction = None
        self.direction_history = deque()
        self.last_direction = None
        
    def GetDirection(self):
        '''Returns 1 if up, 0 if stopped, -1 if down'''
        if self.last_direction == Chamber.BELT_UP:
            return 1
        elif self.last_direction == Chamber.BELT_DOWN:
            return -1
        return 0
    
    def IsSet(self):
        '''Returns True if someone has called Run/Remove since the
           last time that Update() was called'''
        return self.direction is not None
    
    def IsFull( self ):
        '''Returns True if there is a ball present in the chamber'''
        return self.ball_sensor.IsBallPresent()
    
    def Run( self ):
        '''Runs the belt motor to either bring balls into the chamber
           or expel them and move them into the shooter.'''
        self.direction = Chamber.BELT_UP
        
    def Remove(self):
        '''Runs the belt motor to remove balls from the chamber
           back into the feeder'''
        self.direction = Chamber.BELT_DOWN
    
    def Stop( self ):
        '''Stops the belt motor'''
        self.direction = Chamber.BELT_OFF
    
    def Update( self ):
        
        direction = self.direction
        if direction is None:
            direction = Chamber.BELT_OFF
        
        self.belt_motor.Set( direction )
        
        # reset state
        self._calc_last_direction( direction )
        self.direction = None
    
    def _calc_last_direction(self, direction):
        '''Internal function: average belt direction tracking'''
        
        # average the last belt runs to determine whether the balls 
        # are probably traveling up or down
        num = len(self.direction_history) + 1
        
        if num > 6:
            self.direction_history.pop()
        
        d = 0
        
        if direction == Chamber.BELT_DOWN:
            d = -1
        elif direction == Chamber.BELT_UP:
            d = 1
        
        self.direction_history.appendleft(d)
        
        avg = sum(self.direction_history)/num
            
        if avg > 0:
            self.last_direction = Chamber.BELT_DOWN
        elif avg < 0:
            self.last_direction = Chamber.BELT_UP
        else:
            self.last_direction = Chamber.BELT_OFF
        