''' 
Chamber needs to shoot a ball, know when its full, know when empty.
'''

try:
    import wpilib
except ImportError:
    import fake_wpilib as wpilib

class Chamber (object):
    
    def __init__ ( self, chamberRelay, chamberSwitch, chambIRSens ):
        self.chamberMotor = wpilib.Relay(chamberRelay)
        self.limitSwitch3 = wpilib.DigitalInput(chamberSwitch)
        self.switchState = False
        self.chamberMotorState = wpilib.Relay.kOff
        
    def Run( self ):
        self.chamberMotorState = wpilib.Relay.kOn 
        
    def IsReady( self ):
        return self.switchState
        
    def Stop( self ):
        self.chamberMotorState = wpilib.Relay.kOff
    
    def Print( self ):
        print( self.chamberMotorState, self.switchState )
    
    def Update( self ):
        self.chamberMotor.Set( self.chamberMotorState ) # so the update is changing if the motor is running
        self.switchState = self.limitSwitch3.Get()
        