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
        #self.gyro = wpilib.Gyro(susanGyro)
        #self.bodyGyro = wpilib.Gyro(bodyGyro)
        
        #It seems that you forgot to put a self to the right
        #self.bodyGyroInitPos = self.bodyGyro.GetAngle()
        self.susanSource = SusanSource()
        
        self.pidControl = wpilib.PIDController(Susan.SUSAN_P, Susan.SUSAN_I, Susan.SUSAN_D, self.susanSource, self.susanMotor)
        self.pidControl.SetInputRange(-(Susan.SUSAN_CAMERA_MAX),Susan.SUSAN_CAMERA_MAX)
        self.pidControl.SetOutputRange(-(Susan.SUSAN_MAX_SPEED), Susan.SUSAN_MAX_SPEED)
        self.pidControl.SetTolerance(Susan.ANGLETOLERANCE)
        
        self.goalAngle = 0
        self.vBus = 0
        self.autoMode = True
                    
    '''set goal Angle variable'''
    def SetGoalAngle(self,angle):
        self.goalAngle = angle
        
    def SetVBus(self, vBus):
        self.vBus = vBus
        
    def SetMode(self, Auto):
        pass
    
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
        print("    Susan pVoltage:  "+ str(self.pidControl.Get()) + "  Susan GoalAngle: " + str(self.goalAngle) + "  Pointing Correctly: " + str(self.PointingCorrectly()))
        print("    AutoMode: " + str(self.autoMode) + "  Susan vBus: " + str(self.vBus))
    
    '''updates variables'''
    def Update(self):
        
        enable_automode = False
        
        #if you set PVBus go into manual
        if self.vBus is not None:
            autoMode = False
        if self.SetGoalAngle is not None:
            autoMode = True
        else:
            autoMode = False
       
        #detect transitions 
        if autoMode != self.autoMode:
            if not autoMode:
                self.pidControl.Disable()
            else:
                enable_automode = True
            self.autoMode = autoMode
            
            if self.autoMode:
                ''' we always try to get the goalAngle to zero '''
                self.pidControl.SetSetpoint(0)
            
            self.susanSource.setValue(self.goalAngle)

        else:
            self.susanMotor.Set(self.vBus)
            
        if enable_automode:
            self.pidControl.Enable 
        
        #reset Variables   
        self.goalAngle = None
        self.vBus = None
        