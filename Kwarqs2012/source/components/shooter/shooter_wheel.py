
from components.shooter.pidshooter import ShooterWheelOutput

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


def _configure_shooter_motor( motor ):

    motor.SetPositionReference( wpilib.CANJaguar.kPosRef_QuadEncoder )
    motor.ConfigEncoderCodesPerRev( Wheel.ENCODER_TURNS_PER_REVOLUTION )
    motor.ConfigNeutralMode( wpilib.CANJaguar.kNeutralMode_Coast )
         
class Wheel(object):
    
    ENCODER_TURNS_PER_REVOLUTION = 360
    ENCODERTOWHEELRATIO = 4
    WHEEL_FULL_ROTATION = ENCODER_TURNS_PER_REVOLUTION * ENCODERTOWHEELRATIO
    SHOOTER_MOTOR_P = 0.1
    SHOOTER_MOTOR_I = 0.0
    SHOOTER_MOTOR_D = 0.0
    MAXVPercent = 1
    ''' percent tolerance'''
    TOLERANCE = 1
    
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 
    def __init__(self,wheelCAN1, wheelCAN2, encoder):
        self.goalRPM = 0
        self.vBus = 0
        
        self.wheelMotor1 = wpilib.CANJaguar(wheelCAN1)
        self.wheelMotor2 = wpilib.CANJaguar(wheelCAN2)
        self.shooterWheelOutput = ShooterWheelOutput(self.wheelMotor1, self.wheelMotor2)
        
        self.autoMode = True
        _configure_shooter_motor(self.wheelMotor1)
        _configure_shooter_motor(self.wheelMotor2)
        
        self.encoder = wpilib.Encoder(encoder[0],encoder[1])
        self.encoder.Start()
        self.encoder.SetPIDSourceParameter(wpilib.Encoder.kRate)
        self.encoder.SetDistancePerPulse(1.0 / 16.0)
        
        self.encoderShim = EncoderShim(self.encoder)
        
        self.pidControl = wpilib.PIDController(Wheel.SHOOTER_MOTOR_P, Wheel.SHOOTER_MOTOR_I, Wheel.SHOOTER_MOTOR_D, self.encoderShim,  self.shooterWheelOutput)
        self.pidControl.SetOutputRange(-(Wheel.MAXVPercent), Wheel.MAXVPercent)
        self.pidControl.Enable()

            
    '''   
        Description: sets variable that tells update to set the speed

        Variables:    
            float speed (-1 to 1) - speed to set the motor
    '''
    def SetRPM(self,goalRPM):
        self.goalRPM = goalRPM
    
    ''' Set motor vbus directly (-1 to 1) '''
            
    def SetVBus(self, vBus):
        self.vBus = vBus
        
    '''checks if current speed = goal speed if true shooter is ready'''
    def IsReady(self):
        if self.wheelMotor1.GetSpeed() == self.goalSpeed:
            return True
    
    def SetMode(self, Auto):
        self.autoMode = Auto
        if self.autoMode == True:
            self.pidControl.Enable()
        else:
            self.pidControl.Disable()
            
    def Print(self):
        print( "ShooterWheel:")
        print( "    Wheel RPM: " + str(self.encoder.GetRate()) + " Goal RPM: " + str(self.goalRPM) + "Auto Mode: " + str(self.autoMode))
        
    '''update the current speed and goal speed'''
    def Update(self):
        if(self.autoMode):
            self.pidControl.SetSetpoint(self.goalRPM)
        else:
            self.wheelMotor1.Set(self.vBus, ShooterWheelOutput.SYNCGROUP)
            self.wheelMotor2.Set(self.vBus, ShooterWheelOutput.SYNCGROUP)
            wpilib.CANJaguar.UpdateSyncGroup(ShooterWheelOutput.SYNCGROUP)
            
            
    '''
        RM gives # of goalSpeed,mNumber
        mNumber never changes within an instance
        goalSpeed gets updated in update
    '''