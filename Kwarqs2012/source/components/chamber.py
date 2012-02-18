''' 
Chamber needs to shoot a ball, know when its full, know when empty.
'''

try:
    import wpilib
except ImportError:
    import fake_wpilib as wpilib

class Chamber (object):

    def __init__ ( self, chamberRelayNum, limitSwitchNum ):
        chamberMotor = wpilib.Relay(chamberRelayNum)
        limitSwitch3 = wpilib.DigitalInput(limitSwitchNum)
        switchState = False
        
    def Release( self ):
        chamberMotorState = wpilib.Relay.kOn 
        
    def IsReady( self ):
        return switchState
        
    def Stop( self ):
        chamberMotorState = wpilib.Relay.kOff
    
    def Print( self ):
        print( chamberMotorState, switchState )
    
    def Update( self ):
        chamberMotor.Set( chamberMotorState ) # so the update is changing if the motor is running
        switchState = limitSwitch3.Get()
        