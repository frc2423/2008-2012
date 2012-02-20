from ez_can_jaguar import EzCANJaguar
from components.shooter.pidshooter import ShooterWheelOutput

try:
    import wpilib
except ImportError:
    import fake_wpilib as wpilib


def _configure_shooter_motor( motor ):

    motor.SetPositionReference( wpilib.CANJaguar.kPosRef_QuadEncoder )
    motor.ConfigEncoderCodesPerRev( Wheel.ENCODER_TURNS_PER_REVOLUTION )
    motor.ConfigNeutralMode( wpilib.CANJaguar.kNeutralMode_Coast )
         
class Wheel(object):
    
    ENCODER_TURNS_PER_REVOLUTION = 360
    ENCODERTOWHEELRATIO = 4
    WHEEL_FULL_ROTATION = ENCODER_TURNS_PER_REVOLUTION * ENCODERTOWHEELRATIO
    SHOOTER_MOTOR_P = 100.0
    SHOOTER_MOTOR_I = 0.0
    SHOOTER_MOTOR_D = 0.0
    MAXVPercent = 1
    ''' precent tolerance'''
    TOLERANCE = 1
    
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 
    def __init__(self,wheelCAN1, wheelCAN2, encoder):
        self.currentSpeed = 0
        self.goalRPM = 0
        self.vBus = 0
        
        self.wheelMotor1 = EzCANJaguar(wheelCAN1)
        self.wheelMotor2 = wpilib.CANJaguar(wheelCAN2)
        self.shooterWheelOutput = ShooterWheelOutput(self.wheelMotor1, self.wheelMotor2)
        
        self.encoder = wpilib.Encoder(encoder)
        self.encoder.SetPIDSourceParameter(wpilib.Encoder.PIDSourceParameter.kRate)
        self.encoder.SetDistancePulse(1/Wheel.WHEEL_FULL_ROTATION)
        
        pidControl = wpilib.PIDController(Wheel.SHOOTER_MOTOR_P, Wheel.SHOOTER_MOTOR_P, Wheel.SHOOTER_MOTOR_P, self.encoder,  self.shooterWheelOutput)
        pidControl.SetOutputRange(-(Wheel.MAXVPercent), Wheel.MAXVPercent)
        
        self.AutoMode = True
        _configure_shooter_motor(self.wheelMotor1)
        _configure_shooter_motor(self.wheelMotor2)
            
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
        self.AutoMode = Auto
        if self.AutoMode == True:
            self.pidControl.Enable()
        else:
            self.pidControl.Disable()
            
    def Print(self):
        print( "ShooterWheel:")
        print( "    Wheel RPM: " + self.encoder.GetRate() + " Goal RPM: " + self.goalRPM + "Auto Mode: " + self.AutoMode)
        
    '''update the current speed and goal speed'''
    def Update(self):
        if(self.AutoMode):
            self.pidControl.SetSetPoint(self.goalSpeed)
            self.shooterWheelOutput.UpdateMotors()
        else:
            self.wheelMotor1.Set(self.vBus, ShooterWheelOutput.SYNCGROUP)
            self.wheelMotor2.Set(self.vBus, ShooterWheelOutput.SYNCGROUP)
            wpilib.CANJaguar.UpdateSyncGroup(ShooterWheelOutput.SYNCGROUP)
            
            
    '''
        RM gives # of goalSpeed,mNumber
        mNumber never changes within an instance
        goalSpeed gets updated in update
    '''