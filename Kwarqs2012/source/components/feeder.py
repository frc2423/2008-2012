'''
The feeder is meant to move balls from the ground to the chamber, where the ball will then be fired 

'''
try:
    import wpilib
except ImportError:
    import fake_wpilib as wpilib


class Feeder(Object, relayChannel): 
    
    '''
    description: initializes an instance of Feeder
    Variables: 
    relayChannel- The channel that the relay is connected to at the digital I/O
    direction- alters the relay state, which in turn changes the direction the feedermotor rotates   
    '''
    
    def __init__(self,feedmotor1):
        self.feederMotor = Relay(relayChannel)
        self.Full = 0
        self.direction = kOff
    
    #Starts feederMotor if needed
    def FeedOveride(self):
        self.direction = kForward
    
    #Stops the feedermotor if the feeder should not be on
    def Stop(self):
        self.direction = kOff    
    
    
    def Feed(self):
        self.direction = kForward
    
    
    #Allows the feeder to run backwards if giving balls to teamates is desired
    def Expel(self):
        self.direction = kReverse
    
    #def IsFull(self):
    #    if limitswitch == True 
    #    
   

'''   
    def Update(self):
        if FeedOveride() != 1 
            self.feederMotor.set(self.direction)
'''     