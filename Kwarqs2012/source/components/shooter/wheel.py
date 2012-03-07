
from components.shooter.pidshooter import ShooterWheelOutput
from ez_can_jaguar import EzCANJaguar

try:
    import wpilib
except ImportError:
    import fake_wpilib as wpilib
    
    
class EncoderShim(wpilib.PIDSource):
    def __init__(self, encoder):
        wpilib.PIDSource.__init__(self)
        self.encoder = encoder
        
    def PIDGet(self):
        return self.encoder.PIDGet() 

         
class Wheel(object):
    
    ENCODER_TURNS_PER_REVOLUTION = 360
    ENCODERTOWHEELRATIO = 4
    WHEEL_FULL_ROTATION = ENCODER_TURNS_PER_REVOLUTION * ENCODERTOWHEELRATIO
    SHOOTER_MOTOR_P = 0.000675
    SHOOTER_MOTOR_I = 0.000000050
    SHOOTER_MOTOR_D = 0.01
    MAXVPercent = 1
    ''' percent tolerance'''
    TOLERANCE = 1
    
    def __init__(self,wheelCAN1, wheelCAN2, encoder_tuple):
        self.goalRPM = None
        self.vBus = None
        
        self.wheelMotor1 = EzCANJaguar(wheelCAN1)
        self.wheelMotor2 = EzCANJaguar(wheelCAN2)
        
        self.wheelMotor1.ConfigNeutralMode( wpilib.CANJaguar.kNeutralMode_Coast )
        self.wheelMotor2.ConfigNeutralMode( wpilib.CANJaguar.kNeutralMode_Coast )
        
        self.shooterWheelOutput = ShooterWheelOutput(self.wheelMotor1, self.wheelMotor2)
        
        self.autoMode = False
        
        self.encoder = wpilib.Encoder(encoder_tuple[0],encoder_tuple[1])
        self.encoder.SetPIDSourceParameter(wpilib.Encoder.kRate)
        self.encoder.SetDistancePerPulse(1.0 / 16.0)
        self.encoder.Start()
        
        self.encoderShim = EncoderShim(self.encoder)
        
        self.pidControl = wpilib.PIDController(Wheel.SHOOTER_MOTOR_P, Wheel.SHOOTER_MOTOR_I, Wheel.SHOOTER_MOTOR_D, self.encoderShim,  self.shooterWheelOutput)
        #self.pidControl.SetOutputRange(-(Wheel.MAXVPercent), Wheel.MAXVPercent)
        
        # dont enable the PID controller here

            
    '''   
        Description: sets variable that tells update to set the speed

        Variables:    
            float speed (-1 to 1) - speed to set the motor
    '''
    def SetRPM(self,goalRPM):
        self.goalRPM = goalRPM
    
    
            
    def SetVBus(self, vBus):
        ''' Set motor vbus directly (-1 to 1) '''
        self.vBus = vBus
        
    
    def IsReady(self):
        '''Returns True if the shooter is ready for a ball to be fed into it'''
        if self.autoMode == False:
            return True
            
        # TODO: Not sure if this is actually done right
        return self.pidControl.OnTarget()
        
    def SetMode(self, Auto):
        pass
        
            
    def Print(self):
        print( "ShooterWheel:")
        print( "    Wheel RPM: %f; Goal RPM: %s; Auto Mode: %s" % (self.encoder.GetRate(), self.goalRPM, self.autoMode))
        
    '''update the current speed and goal speed'''
    def Update(self):
    
        enable_automode = False
    
        # if you set vbus, you win
        if self.vBus is not None:
            autoMode = False
        if self.goalRPM is not None:
            autoMode = True
        else:
            autoMode = False
            
        # detect transitions to/from automode
        if autoMode != self.autoMode:
            if not autoMode:
                self.pidControl.Disable()
            else:
                enable_automode = True
            self.autoMode = autoMode
    
        if self.autoMode:
            self.pidControl.SetSetpoint(self.goalRPM)
        else:
            if self.vBus is None:
                self.vBus = 0
            
            self.wheelMotor1.Set(self.vBus, ShooterWheelOutput.SYNCGROUP)
            self.wheelMotor2.Set(-self.vBus, ShooterWheelOutput.SYNCGROUP)
            wpilib.CANJaguar.UpdateSyncGroup(ShooterWheelOutput.SYNCGROUP)
            
        if enable_automode:
            self.pidControl.Enable()
            
        # reset vars at the end
        self.goalRPM = None
        self.vBus = None
            
            
    '''
        RM gives # of goalSpeed,mNumber
        mNumber never changes within an instance
        goalSpeed gets updated in update
    '''