
try:
    import wpilib
    import wpilib.SmartDashboard
except ImportError:
    import fake_wpilib as wpilib
    import fake_wpilib.SmartDashboard

import threading
    

class SusanSource(wpilib.PIDSource):
    '''Source for the Susan PID controller'''
    
    def __init__(self):
        wpilib.PIDSource.__init__(self)
        self._lock = threading.RLock()
        self.val = 0.0
    
    def Set(self, val):
        with self._lock:
            self.val = val
    
    def PIDGet(self):
        with self._lock:
            return self.val
    
    
class SusanPidOutput(wpilib.PIDOutput):
    '''Jaguar output for susan PID'''
    
    def __init__(self, jaguar):
        wpilib.PIDOutput.__init__(self)
        self.jaguar = jaguar

    def PIDWrite(self, output):
        self.jaguar.Set(output)
        
    

class Susan(object):
    '''
        The top of our robot has a shooter, which is mounted on a lazy
        susan. There is a motor attached to the susan which allows it 
        to turn *almost* 360 degrees. Its like.. 320 or something. 
        
        TODO: We don't handle the case where the susan reaches the end
        of its limit switches in automated mode
        
        TODO: Need to figure out semantics for gyro-based angle control
    '''
    
    # these values are probably too high
    SUSAN_P = 1.21
    SUSAN_I = 0.001
    SUSAN_D = 0.001
    SUSAN_CAMERA_MAX = 20
    SUSAN_MAX_SPEED = 1
    ANGLE_TOLERANCE = 5      # the percent tolerance of the angle to be considered ready'''
    
    def __init__(self, susanCAN, susanGyro, bodyGyro):
        
        self.susanMotor = wpilib.CANJaguar(susanCAN)
        
        # problem: when you initialize a gyro, it sits there for 6 seconds
        # and doesn't let the robot do anything. How do we deal with this
        # problem? 
        #self.gyro = wpilib.Gyro(susanGyro)
        #self.bodyGyro = wpilib.Gyro(bodyGyro)
        
        #It seems that you forgot to put a self to the right
        #self.bodyGyroInitPos = self.bodyGyro.GetAngle()
        self.pid_source = SusanSource()
        self.pid_output = SusanPidOutput(self.susanMotor)
        
        self.pidControl = wpilib.SmartDashboard.SendablePIDController(Susan.SUSAN_P, Susan.SUSAN_I, Susan.SUSAN_D, self.pid_source, self.pid_output)
        self.pidControl.SetInputRange(-(Susan.SUSAN_CAMERA_MAX),Susan.SUSAN_CAMERA_MAX)
        self.pidControl.SetOutputRange(-(Susan.SUSAN_MAX_SPEED), Susan.SUSAN_MAX_SPEED)
        self.pidControl.SetTolerance(Susan.ANGLE_TOLERANCE)
        
        # this never changes, when we're using PID control the goal is to get
        # the offset to zero
        self.pidControl.SetSetpoint(0.0)
        
        # warning: never enable the pid controller here
        
        self.angle = None
        self.vBus = None
        
        # this starts as None so that when update gets called we
        # can immediately set the current mode regardless of the input
        self.autoMode = None
                    
        self.sd = wpilib.SmartDashboard.SmartDashboard.GetInstance()
        self.sd.PutData( "Susan PID", self.pidControl )
    
    def SetAutoRelativeAngle(self,angle):
        '''When called, this tells the susan to automatically go to an angle
           relative to where the susan is currently pointing'''
        self.angle = angle
        
        # TODO: If we use a gyro, this is no longer correct
        
    def SetVBus(self, value):
        '''Manually control the susan. value is [-1,1]'''
        self.vBus = value
        
    # TODO: Add a way for the user to adjust the angle, then hold it using the
    # gyro. This allows us to avoid the susan vibrating off its setpoint.
    # ... maybe
        
    
    def PointingCorrectly(self):
        '''
            start pointing at wall temporary return true 
        '''
        
        # if there was a gyro here, we would use this to figure it out
    
        return True 
            
    
    def IsReady(self):
        '''
            Returns True when:

                - automode is off
                - or the susan angle is very near the goal angle and 
                the susan is pointing in the direction of the goal
        '''
        
        if self.autoMode == False:
            return True
        
        return self.PointingCorrectly() and self.pidControl.OnTarget()
        
        
    def IsSet(self):
        return self.angle is not None or self.vBus is not None
        
    
    def Print(self):
        # TODO: Implement this when there is a unified format to print this.. 
        pass
        
        #print("ShooterSusan: ")
        #print("    Susan pVoltage:  "+ str(self.pidControl.Get()) + "  Susan GoalAngle: " + str(self.goalAngle) + "  Pointing Correctly: " + str(self.PointingCorrectly()))
        #print("    AutoMode: " + str(self.autoMode) + "  Susan vBus: " + str(self.vBus))
    
    '''updates variables'''
    def Update(self):
        
        enable_automode = False
        
        #if you set PVBus go into manual
        if self.vBus is not None:
            autoMode = False
        if self.angle is not None:
            autoMode = True
        else:
            autoMode = False
       
        #detect transitions to/from automode
        if autoMode != self.autoMode:
            if not autoMode:
                self.pidControl.Disable()
            else:
                enable_automode = True
            self.autoMode = autoMode
            
        
        if self.autoMode:
            self.pid_source.Set( self.angle )
            
        else:
            if self.vBus is None:
                self.vBus = 0
        
            self.susanMotor.Set(self.vBus)
        
        # do this last otherwise the PID control might be enabled with a nonsense goal
        if enable_automode:
            self.pidControl.Enable()
        
        #reset variables   
        self.angle = None
        self.vBus = None
        