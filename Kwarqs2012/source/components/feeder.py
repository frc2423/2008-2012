'''
The feeder is meant to move balls from the ground to the chamber, where the
ball will then be fired 

'''

from components.irsensor import IRSensor
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
    
    lowSwitch1- state of limit switch 1
    topSwitch2- state of limit switch 2 (higher one)
    '''
    FORWARD = 1
    BACKWARD = -1
    STOP = 0
    
    def __init__(self, feederJag, topFeedSwitch, botFeedSwitch, topFeedIRSens):
        
        self.feederMotor = wpilib.Jaguar(feederJag)

        #set so that feeder is naturally on. May change
        self.direction = Feeder.FORWARD
        self.botFeedSwitch = wpilib.DigitalInput( botFeedSwitch )
        self.topFeedSwitch = wpilib.DigitalInput( topFeedSwitch )
        
        #not sure switches will be used
        self.botSwitchVal = False
        self.topSwitchVal = False
        
        self.topFeedIRSens = IRSensor(topFeedIRSens)
        #If implementing another IR
        #self.botFeedIRSens = IRSensor(botFeedIRSens)
        
        self.Full = False
    #Starts feederMotor if needed
    def Feed(self):
        self.direction = Feeder.FORWARD
        
        
    #Stops the feedermotor if the feeder should not be on
    def Stop(self):
        self.direction = Feeder.BACKWARD    
    
    """
    def FeedIfBallNear(self):
        if 
        self.direction = kForward
        # ^ Need a proximity sensor to register for this to work- change
    """
    
    #Allows the feeder to run backwards if giving balls to teamates is desired
    def Expel(self):
        self.direction = Feeder.STOP
    
    #Returns 1 if the feeder is full and 0 if not
    def IsFull(self):
        if self.botFeedSwitch.Get() == True and self.topFeedIRSens.isBallSet() == True:
            return( True )
        
    def IsReady(self):
        if self.topFeedIRSens.isBallSet() == True:
            return( True )
        
    def Print(self):
            print("Feeder:")
            print("    IRSensor Value: " + str(self.topFeedIRSens.irSensor.GetVoltage()) + "  Is Ball Set: " + str(self.topFeedIRSens.isBallSet()))
            print("    Is full: " + str(self.IsFull()) + "  IsReady: " +str(self.IsReady()) + "    Motor State")
    '''
    Sets the feeder motor's direction (as well as whether it is on)
    Also updates the state of the limit switches
    '''
    def Update(self):
        self.feederMotor.Set(self.direction)
        '''
        going to do this with the actuall values
        
        self.switch1 = self.limSwitch1.Get()
        self.switch2 = self.limSwitch2.Get()
        '''
        
      
        
        
        
    