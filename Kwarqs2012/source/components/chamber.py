''' 
Chamber needs to shoot a ball, know when its full, know when empty.
'''

try:
    import wpilib
except ImportError:
    import fake_wpilib as wpilib

class Chamber ( Object ):

    def __Init__ ( self, chamberCANJagNum, limitSwitchNum ):
        chamberMotor = CANJaguar(chamberCANJagNum)
        limitSwitch3 = DigitalInput(limitSwitchNum)
        switchState = False
        
    def Release( self ):
        chamberMotorState = 1.0      
        
    def IsReady( self ):  
        return switchState
        
    def Stop( self ):
        chamberMotorState = 0.0
        
    def Update( self ):
        chamberMotor.Set( chamberMotorState ) # so the update is changing if the motor is running
        switchState = limitSwitch3.Get()
        