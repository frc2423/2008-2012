'''
    electrical_test 
    
    This program is for the Kwarqs 2012 robot. It is intended to be the 
    most basic program possible to exercise all of the electrical
    functionality of the robot. By having something simple that *just works*,
    we can test the robot when it is having problems and determine
    if the problems are mechanical, electrical, or program-related. 
    
    Controls:
    
        Stick1:
        
            X/Y: drive motors
            Trigger: ramp motor
            Z: angle
            
        Stick2:
        
            Trigger: shooter (z axis)
            Buttons 8,9: lazy susan
            Buttons 6,7: feeder
            Buttons 10,11: feeder

'''

# print something out so we know what code is running
print( "Loading: Kwarqs2012 electrical testing program" )

try:
    import wpilib
except ImportError:
    import fake_wpilib as wpilib

# our custom python module for tracking baskets
#from basket_tracker import BasketTracker

# TODO: Give these better names to reflect what they do
# TODO: Add sensors as needed
# TODO: Organize the motors and sensors better

# Joysticks
stick1 = wpilib.Joystick(1)
stick2 = wpilib.Joystick(2)

# PWM motor controllers (drive motors)
l_motor = wpilib.Jaguar(2)
r_motor = wpilib.Jaguar(1)
feeder_motor = wpilib.Jaguar(3)
camera_servo = wpilib.Servo(5)
camera_relay = wpilib.Relay(3)


# CAN motor controllers
shooter_motor1 = wpilib.CANJaguar(2)
shooter_motor2 = wpilib.CANJaguar(3)
angle_motor = wpilib.CANJaguar(4)
ramp_motor = wpilib.CANJaguar(5)
susan_motor = wpilib.CANJaguar(6)

shooter_encoder = wpilib.Encoder(10,11)

chamber_sensor = wpilib.AnalogChannel( 5 )
feeder_sensor = wpilib.AnalogChannel( 6 )

gyro1 = wpilib.Gyro(1)
gyro2 = wpilib.Gyro(2)

# print firmware versions
print( "CANJaguar Firmware: \n" +
       ("   Shooter1 (CAN2): %d\n" % shooter_motor1.GetFirmwareVersion() ) +
       ("   Shooter2 (CAN3): %d\n" % shooter_motor2.GetFirmwareVersion() ) +
       ("   Angle    (CAN4): %d\n" % angle_motor.GetFirmwareVersion() ) +
       ("   Ramp     (CAN5): %d\n" % ramp_motor.GetFirmwareVersion() ) +
       ("   Susan    (CAN6): %d\n" % susan_motor.GetFirmwareVersion() )
    )

# relays
relay1 = wpilib.Relay(1)
chamber_relay = wpilib.Relay(2)
relay4 = wpilib.Relay(4)

# switches
switch1 = wpilib.DigitalInput(1)
switch2 = wpilib.DigitalInput(2)
switch3 = wpilib.DigitalInput(3)

# vision basket tracking code
#tracker = BasketTracker()

# Angle motor related stuff

ANGLE_MOTOR_MIN_POSITION = 0.0
ANGLE_MOTOR_MAX_POSITION = 1.0
ANGLE_MOTOR_P = 250.0
ANGLE_MOTOR_I = 0.0
ANGLE_MOTOR_D = 0.0

ENABLE_ANGLE_MOTOR = True

def _translate_z_to_angle_motor_position( z ):

    # P = Xmax - (Ymax - Y)( (Xmax - Xmin) / (Ymax - Ymin) )
    value = ANGLE_MOTOR_MAX_POSITION - ((1 - z)*( (ANGLE_MOTOR_MAX_POSITION - ANGLE_MOTOR_MIN_POSITION) / (1.0-0) ) )
    
    if value > ANGLE_MOTOR_MAX_POSITION:
        return ANGLE_MOTOR_MAX_POSITION
    elif value < ANGLE_MOTOR_MIN_POSITION:
        return ANGLE_MOTOR_MIN_POSITION
    return value
        

# shooter wheel stuff
ENCODER_TURNS_PER_REVOLUTION = 360
        
SHOOTER_MOTOR_P = 100.0
SHOOTER_MOTOR_I = 0.0
SHOOTER_MOTOR_D = 0.0


def _configure_shooter_motor( motor ):

    motor.SetPositionReference( wpilib.CANJaguar.kPosRef_QuadEncoder )
    motor.ConfigEncoderCodesPerRev( ENCODER_TURNS_PER_REVOLUTION )
    motor.ConfigNeutralMode( wpilib.CANJaguar.kNeutralMode_Coast )
    
    # enable PID based speed control of the motor.. 
    # motor.ChangeControlMode( wpilib.CANJaguar.kSpeed )
    # motor.SetPID( SHOOTER_MOTOR_P, SHOOTER_MOTOR_I, SHOOTER_MOTOR_D )
    # motor.EnableControl()

    
    
class MyRobot(wpilib.SimpleRobot):

    def __init__(self):
        '''Constructor'''
        
        wpilib.SimpleRobot.__init__(self)
        self.drive = wpilib.RobotDrive( l_motor, r_motor )
        
        angle_motor.ConfigNeutralMode( wpilib.CANJaguar.kNeutralMode_Brake )
        angle_motor.SetPositionReference( wpilib.CANJaguar.kPosRef_Potentiometer )
        angle_motor.ConfigPotentiometerTurns( 1 )
        
        if ENABLE_ANGLE_MOTOR:
            angle_motor.ChangeControlMode( wpilib.CANJaguar.kPosition )
            angle_motor.SetPID( ANGLE_MOTOR_P, ANGLE_MOTOR_I, ANGLE_MOTOR_D )
            angle_motor.EnableControl()
            
        _configure_shooter_motor( shooter_motor1 )
        _configure_shooter_motor( shooter_motor2 )
        
        shooter_encoder.SetDistancePerPulse( 1.0 / 16.0 )
        shooter_encoder.Start()
        camera_relay.Set( wpilib.Relay.kForward )
            
    def RobotInit(self):
        '''Called only once during robot initialization'''
        pass

    def Disabled(self):
        '''Called when the robot is disabled'''
        
        print("MyRobot::Disabled()")
    
        while self.IsDisabled():
            wpilib.Wait(0.01)

    def Autonomous(self):
        '''Called during autonomous mode'''
        
        print("MyRobot::Autonomous()")
    
        self.GetWatchdog().SetEnabled(False)
        while self.IsAutonomous() and self.IsEnabled():
            wpilib.Wait(0.01)
            
            
    def OperatorControl(self):
        '''Called during Teleoperated Mode'''
    
        print("MyRobot::OperatorControl()")
    
        dog = self.GetWatchdog()
        dog.SetEnabled(True)
        dog.SetExpiration(0.25)
        
        timer = wpilib.Timer()
        timer.Start()

        # main loop
        while self.IsOperatorControl() and self.IsEnabled():
   
            dog.Feed()
            
            translated_z =  _translate_z_to_angle_motor_position( stick1.GetZ() )
            
            #
            # Drive motors
            #
            
            self.drive.ArcadeDrive( stick1 )
            
            #
            # Ramp motor
            #
            
            if stick1.GetRawButton(11):
                ramp_motor.Set( 1.0 ) # down
            elif stick1.GetRawButton(10):
                ramp_motor.Set( -.45 ) #up
            else:
                ramp_motor.Set( 0.0 )
            
            #
            # Shooter motor
            #
            
            if stick2.GetTrigger():
                z = stick2.GetZ()
                shooter_motor1.Set( z )
                shooter_motor2.Set( -z )
            else:
                shooter_motor1.Set( 0.0 )
                shooter_motor2.Set( 0.0 )
                
            #
            # Angle motor
            #
            
            #if ENABLE_ANGLE_MOTOR:
            #    angle_motor.Set( _translate_z_to_angle_motor_position( stick1.GetZ() ) )
            
            camera_servo.Set( translated_z )
            
            #
            # Feeder
            #
             
            if stick2.GetRawButton(6):      # up
                feeder_motor.Set( -1.0 )
            elif stick2.GetRawButton(7):    # down
                feeder_motor.Set( 1.0 )
            else:
                feeder_motor.Set( 0 )
                
            #
            # Chamber
            #
                
            if stick2.GetRawButton(10):      # up
                chamber_relay.Set( wpilib.Relay.kForward )
            elif stick2.GetRawButton(11):    # down
                chamber_relay.Set( wpilib.Relay.kReverse )
            else:
                chamber_relay.Set( wpilib.Relay.kOff )
                
            #
            # Lazy susan
            #
            
            if stick2.GetRawButton(8):      # left
                susan_motor.Set( 1.0 )
            elif stick2.GetRawButton(9):    # right
                susan_motor.Set( -1.0 )
            else:
                susan_motor.Set( 0.0 )
            
           
            
            
            if timer.HasPeriodPassed( 1.0 ):
                # TODO: Print out something useful here to help us diagnose problems with the robot
                print( "Status: " )
                print( "  Shooter Motor1: %f; Encoder: %f" % ( shooter_motor1.Get(), shooter_encoder.GetRate() ) )
                print( "  Shooter Motor2: %f" % ( shooter_motor2.Get() ) )
                print( "  Angle Motor   : %f; Encoder: %f" % ( angle_motor.Get(), angle_motor.GetPosition() ) )
                print( "  Feeder switches: 1: %s, 2: %s, 3: %s" % (switch1.Get(), switch2.Get(), switch3.Get() ) )
                print( "  Chamber: %f; Feeder: %f" % (chamber_sensor.GetVoltage(), feeder_sensor.GetVoltage() ))
                print( "  Gyro1: %f, Gyro2: %f" % (gyro1.GetAngle(), gyro2.GetAngle()) )
                print( "" )
                
                
            
            wpilib.Wait(0.04)

        
def run():
    '''This function must be present for the robot to start'''
    robot = MyRobot()
    robot.StartCompetition()
    
    # must return a value to work with the tester program
    return robot



