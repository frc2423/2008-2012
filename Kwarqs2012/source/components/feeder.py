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
        self.Full = False
        
    #Starts feederMotor if needed
    def Feed(self):
        self.direction = Feeder.FORWARD
        
    #Stops the feedermotor if the feeder should not be on
    def Stop(self):
        self.direction = Feeder.STOP
    
    #Allows the feeder to run backwards if giving balls to teamates is desired
    def Expel(self):
        self.direction = Feeder.BACKWARD
    
    #Returns 1 if the feeder is full and 0 if not
    def IsFull(self):
        if self.botFeedSwitch.Get() == True and self.topFeedIRSens.IsBallSet() == True:
            return True
        return False
        
    def IsReady(self):
        if self.topFeedIRSens.IsBallSet() == True:
            return True
        return False
        
    def Print(self):
            print("Feeder:")
            print("    IRSensor Value: %s; Is Ball Set: %s" % ( self.topFeedIRSens.GetVoltage(), self.topFeedIRSens.IsBallSet() ))
            print("    Is full: %s; IsReady: %s" % ( self.IsFull(), self.IsReady() ))

    def Update(self):
        self.feederMotor.Set(self.direction)
        
      
        
        
        
    