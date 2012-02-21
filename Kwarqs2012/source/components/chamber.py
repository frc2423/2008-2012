''' 
Chamber needs to shoot a ball, know when its full, know when empty.
'''
from components.irsensor import IRSensor
try:
    import wpilib
except ImportError:
    import fake_wpilib as wpilib

class Chamber (object):
    
    def __init__ ( self, chamberRelay, chambIRSens ):
    
        self.chamberMotor = wpilib.Relay(chamberRelay)
        self.chamberMotorState = wpilib.Relay.kOff
        self.chambIRSens = IRSensor(chambIRSens, 2.0)
        
    def Run( self ):
        self.chamberMotorState = wpilib.Relay.kReverse 
        
    def Remove(self):
        self.chamberMotorState = wpilib.Relay.kForward
        
    def IsFull( self ):
        return self.chambIRSens.IsBallSet()
        
    def Stop( self ):
        self.chamberMotorState = wpilib.Relay.kOff
    
    def Print( self ):
        print("Chamber:")
        print("  IRSensor: %s; Is Ball Set: %s" % (self.chambIRSens.GetVoltage(), self.IsFull()))
    
    def Update( self ):
    
        self.chamberMotor.Set( self.chamberMotorState )
        self.chamberMotorState = wpilib.Relay.kOff

        