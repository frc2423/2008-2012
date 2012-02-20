from components.shooter.pidsusan import SusanSource
from ez_can_jaguar import EzCANJaguar
try:
    import wpilib
except ImportError:
    import fake_wpilib as wpilib



class Susan(object):
    
    SUSAN_P = 1.0
    SUSAN_I = 0.0
    SUSAN_D = 0.0
    SUSAN_CAMERA_MAX = 20
    SUSAN_MAX_SPEED = 1
    
    def __init__(self,susanCAN,susanGyro, bodyGyro):
        
        self.susanMotor = EzCANJaguar(susanCAN)
        self.gyro = wpilib.Gyro(susanGyro)
        self.bodyGyro = wpilib.Gyro(bodyGyro)
        
        #It seems that you forgot to put a self to the right
        self.bodyGyroInitPos = self.bodyGyro.GetAngle()
        self.susanSource = SusanSource()
        
        pidControl = wpilib.PIDController(Susan.SUSAN_P, Susan.SUSAN_I, Susan.SUSAN_D, self.susanSource, self.susanMotor)
        pidControl.SetInputRange(-(Susan.SUSAN_CAMERA_MAX),Susan.SUSAN_CAMERA_MAX)
        pidControl.SetOutputRange(-(Susan.SUSAN_MAX_SPEED), Susan.SUSAN_MAX_SPEED)
        
        self.goalAngle = 0
    
    '''
        sets motor into diffrent control states if
        true in overide mode motor in v control
        else in speed control
    '''
    def OverrideMode(self, override):
       if override :
           self.susanMotor.ChangeControlMode(controlMode)
    '''set goal speed variable'''
    def SetGoalAngle(self,angle):
        self.goalAngle = angle
    '''
        start pointing at wall temporary return true 
    '''
    def PointingCorrectly(self):
        return True
            
    '''updates variables'''
    def Update(self):
        self.susanSource(self.goalAngle)