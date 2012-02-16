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
            
            Buttons 10,11: relays
            
        Stick2:
        
            Trigger: shooter (z axis)
            Buttons 8,9: lazy susan
            Buttons 2,3: feeder

'''


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
#camera_servo = wpilib.Jaguar(5)

# CAN motor controllers
shooter_motor1 = wpilib.CANJaguar(2)
shooter_motor2 = wpilib.CANJaguar(3)
angle_motor = wpilib.CANJaguar(4)
ramp_motor = wpilib.CANJaguar(5)
susan_motor = wpilib.CANJaguar(6)

# relays
relay1 = wpilib.Relay(1)
relay2 = wpilib.Relay(2)
relay3 = wpilib.Relay(3)
relay4 = wpilib.Relay(4)

# vision basket tracking code
#tracker = BasketTracker()


class MyRobot(wpilib.SimpleRobot):

    def __init__(self):
        wpilib.SimpleRobot.__init__(self)
        self.drive = wpilib.RobotDrive( l_motor, r_motor )

    def Disabled(self):
        
        print("MyRobot::Disabled()")
    
        while self.IsDisabled():
            wpilib.Wait(0.01)

    def Autonomous(self):
        
        print("MyRobot::Autonomous()")
    
        self.GetWatchdog().SetEnabled(False)
        while self.IsAutonomous() and self.IsEnabled():
            wpilib.Wait(0.01)
            
            
    def OperatorControl(self):
    
        print("MyRobot::OperatorControl()")
    
        dog = self.GetWatchdog()
        dog.SetEnabled(True)
        dog.SetExpiration(0.25)
        
        timer = wpilib.Timer()
        timer.Start()

        # main loop
        while self.IsOperatorControl() and self.IsEnabled():
   
            dog.Feed()
            
            #
            # Drive motors
            #
            
            self.drive.ArcadeDrive( stick1 )
            
            #
            # Ramp motor
            #
            
            if stick1.GetTrigger():
                angle_motor.Set( 1.0 )
            else:
                angle_motor.Set( -1.0 )
            
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
            # Feeder
            #
             
            if stick2.GetRawButton(3):      # up
                feeder_motor.Set( 1.0 )
            elif stick2.GetRawButton(2):    # down
                feeder_motor.Set( -1.0 )
            else:
                feeder_motor.Set( 0 )
                
            #
            # Lazy susan
            #
            
            if stick2.GetRawButton(8):      # left
                susan_motor.Set( 1.0 )
            elif stick2.GetRawButton(9):    # right
                susan_motor.Set( -1.0 )
            else:
                susan_motor.Set( 0.0 )
            
            #
            # Relays
            #
            
            if stick1.GetRawButton(10):
                relay1.Set( wpilib.Relay.kForward )
                relay2.Set( wpilib.Relay.kForward )
                relay3.Set( wpilib.Relay.kForward )
                relay4.Set( wpilib.Relay.kForward )
            
            elif stick1.GetRawButton(11):
                relay1.Set( wpilib.Relay.kReverse )
                relay2.Set( wpilib.Relay.kReverse )
                relay3.Set( wpilib.Relay.kReverse )
                relay4.Set( wpilib.Relay.kReverse )
            
            else:
                relay1.Set( wpilib.Relay.kOff )
                relay2.Set( wpilib.Relay.kOff )
                relay3.Set( wpilib.Relay.kOff )
                relay4.Set( wpilib.Relay.kOff )
            
            
            if timer.HasPeriodPassed( 1.0 ):
                # TODO: Print out something useful here to help us diagnose problems with the robot
                #print( "Motor: %f; Encoder: %d" % ( motor.Get(), encoder.Get() ) )
                pass
                
            
            wpilib.Wait(0.04)

        
def run():
    '''This function must be present for the robot to start'''
    robot = MyRobot()
    robot.StartCompetition()
    return robot



