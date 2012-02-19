
try:
    import wpilib
except ImportError:
    import fake_wpilib as wpilib

ENCODER_TURNS_PER_REVOLUTION = 360

def _configure_shooter_motor( self, motor ):

    motor.SetPositionReference( wpilib.CANJaguar.kPosRef_QuadEncoder )
    motor.ConfigEncoderCodesPerRev( ENCODER_TURNS_PER_REVOLUTION )
    motor.ConfigNeutralMode( wpilib.CANJaguar.kNeutralMode_Coast )
    
class Wheel(object):
    

    
    def __init__(self,wheelCAN1, wheelCAN2):
        self.currentSpeed = 0
        self.goalSpeed = 0
        self.wheelMotor1 = wpilib.CANJaguar(wheelCAN1)
        self.wheelMotor2 = wpilib.CANJaguar(wheelCAN2)
        _configure_shooter_motor(self.wheelMotor1)
        _configure_shooter_motor(self.wheelMotor2)
    
        
    '''   
        Description: sets variable that tells update to set the speed

        Variables:    
            float speed (-1 to 1) - speed to set the motor
    '''
    def SetSpeed(self,goalSpeed):
        self.goalSpeed = goalSpeed
        
        
    #checks if current speed = goal speed
    def IsReady(self):
        if self.currentSpeed == self.goalSpeed:
            return True
    
    
    #update the current speed and goal speed
    def Update(self):
        self.wheelMotor1(self.goalSpeed)
        self.wheelMotor2(-self.goalSpeed)
        self.currentSpeed = self.wheel.GetSpeed()
        
            
            
    '''
        RM gives # of goalSpeed,mNumber
        mNumber never changes within an instance
        goalSpeed gets updated in update
    '''