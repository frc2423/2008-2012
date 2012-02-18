'''
The feeder is meant to move balls from the ground to the chamber, where the
ball will then be fired 

'''
try:
    import wpilib
except ImportError:
    import fake_wpilib as wpilib


class Feeder(object): 
    
    '''
    description: initializes an instance of Feeder
    Variables: 
    relayChannel- The channel that the relay is connected to at the digital 
    I/O
    
    direction- alters the relay state, which in turn changes the direction the
    feedermotor rotates   
    
    limSwitchChannel1 and limSwitchChannel2- The channels on the digital
    sidecar that the limitswitches correspond to
    
    proxchannel- The channel on the digital sidecar that the proximity
    sensor is connected to
    
    switch1- state of limit switch 1
    switch2- state of limit switch 2 (higher one)
    '''
    
    def __init__(self, relayChannel, limSwitch1Channel, limSwitch2Channel, proxchannel):
        self.feederMotor = wpilib.Relay(relayChannel)
        self.Full = False
        self.direction = wpilib.Relay.kOff
        self.limSwitch1 = wpilib.DigitalInput( limSwitch1Channel )
        self.limSwitch2 = wpilib.DigitalInput( limSwitch2Channel )
        self.switch1 = False
        self.switch2 = False
        
    #Starts feederMotor if needed
    def Feed(self):
        self.direction = wpilib.Relay.kForward
    
    #Stops the feedermotor if the feeder should not be on
    def Stop(self):
        self.direction = wpilib.Relay.kOff    
    
    """
    def FeedIfBallNear(self):
        if 
        self.direction = kForward
        # ^ Need a proximity sensor to register for this to work- change
    """
    
    #Allows the feeder to run backwards if giving balls to teamates is desired
    def Expel(self):
        self.direction = wpilib.Relay.kReverse
    
    #Returns 1 if the feeder is full and 0 if not
    def IsFull(self):
        if self.switch1 == True and self.switch2 == True:
            return( True )
    '''
    This function is meant to return the states of switches in the feeder (when more info. than IsFull is needed)
    The last elif statement may be redundant (due to IsFull)
    '''
    def BallStates(self):
        if self.switch1 == 0 and self.switch2 == 0:
            return( 0 )
        elif self.switch1 == 1 and self.switch2 == 0:
            return( 1 )
        elif self.switch1 == 0 and self.switch2 == 1:
            return( 2 )
        elif self.switch1 == 1 and self.switch2 == 1:
            return( 3 )

    '''
    Sets the feeder motor's direction (as well as whether it is on)
    Also updates the state of the limit switches
    '''
    def Update(self):
        self.feederMotor.Set(self.direction)
        self.switch1 = self.limSwitch1.Get()
        self.switch2 = self.limSwitch2.Get()
        
        
        
        
        
        
    