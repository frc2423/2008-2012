

try:
    import wpilib
except ImportError:
    import fake_wpilib as wpilib

from ..ir_sensor import IRSensor



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
        
    def Print( self ):
        print("Chamber:")
        print("  IRSensor: %s; Is Ball Present: %s" % (self.ball_sensor.GetVoltage(), self.IsFull()))
    
    def Update( self ):
        
        direction = self.direction
        if direction is None:
            direction = Chamber.BELT_OFF
        
        self.belt_motor.Set( direction )
        
        # reset state
        self.direction = None

        