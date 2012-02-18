''' 
Chamber needs to shoot a ball, know when its full, know when empty.
'''

try:
    import wpilib
except ImportError:
    import fake_wpilib as wpilib

class Chamber (object):

    def __Init__ ( self, chamberRelayNum, limitSwitchNum ):
        chamberMotor = Relay(chamberRelayNum)
        limitSwitch3 = DigitalInput(limitSwitchNum)
        switchState = False
        
    def Release( self ):
        chamberMotorState = kOn
        
    def IsReady( self ):
        return switchState
        
    def Stop( self ):
        chamberMotorState = kOff
    
    def Print( self ):
        print( chamberMotorState, switchState )
    
    def Update( self ):
        chamberMotor.Set( chamberMotorState ) # so the update is changing if the motor is running
        switchState = limitSwitch3.Get()
        