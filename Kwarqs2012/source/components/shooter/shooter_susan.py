from components.shooter.pidshooter import SusanSource
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
    ''' the percent tolerance of the angle to be considered ready'''
    ANGLETOLERANCE = 5
    
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
        pidControl.SetTolerance(Susan.ANGLETOLERANCE)
        
        self.goalAngle = 0
        self.goalPVBus = 0
        self.AutoMode = True
                    
    '''set goal Angle variable'''
    def SetGoalAngle(self,angle):
        self.goalAngle = angle
        
    def SetPVbus(self, PVBus):
        self.goalPVBus = PVBus
        
    def SetMode(self, Auto):
        self.AutoMode = Auto
        if self.AutoMode == True:
            self.pidControl.Enable()
        else:
            self.pidControl.Disable()
    '''
        start pointing at wall temporary return true 
    '''
    def PointingCorrectly(self):
        return True 
            
    '''
        define ready when the susans angle is very near the goal angle
        and we are pointing Correctly
    '''
    def IsReady(self):
        return self.PointingCorrectly() and self.pidControl.OnTarget()
    
    def Print(self):
        print("ShooterSusan: ")
        print("    Susan Speed =  "+ self.pidControl.Get() + "  Susan GoalAngle = " + self.goalAngle + "  Pointing Correctly = " + self.PointingCorrectly() )
    
    '''updates variables'''
    def Update(self):
        if(self.AutoMode):
            self.susanSource.setValue(self.goalAngle)
            ''' we always try to get the goalAngle to zero '''
            self.pidControl.SetSetpoint(0)
        else:
            self.susanMotor.Set(self.goalPVBuss)