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
        self.Full = 0
        self.direction = wpilib.Relay.kOff
        limSwitch1 = wpilib.DigitalInput( limSwitch1Channel )
        limSwitch2 = wpilib.DigitalInput( limSwitch2Channel )
        switch1 = False
        switch2 = False
        
    #Starts feederMotor if needed
    def FeedOveride(self):
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
        self.direction = kReverse
    
    #Returns 1 if the feeder is full and 0 if not
    def IsFull(self):
        if switch1 == 1 and switch2 == 1:
            return( 1 )
    '''
    This function is meant to return the states of switches in the feeder (when more info. than IsFull is needed)
    The last elif statement may be redundant (due to IsFull)
    '''
    def BallStates(self):
        if switch1 == 0 and switch2 == 0:
            return( 0 )
        elif switch1 == 1 and switch2 == 0:
            return( 1 )
        elif switch1 == 0 and switch2 == 1:
            return( 2 )
        elif switch1 == 1 and switch2 == 1:
            return( 3 )

    '''
    Sets the feeder motor's direction (as well as whether it is on)
    Also updates the state of the limit switches
    '''
    def Update(self):
        self.feederMotor.Set(self.direction)
        switch1 = limSwitch1.Get()
        switch2 = limSwitch2.Get()
        
        
        
        
        
        
    