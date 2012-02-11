
#import rpdb2

try:
    import wpilib
except ImportError:
    import fake_wpilib as wpilib


stick1 = wpilib.Joystick(1)
stick2 = wpilib.Joystick(2)

# this is the motor attached to the encoder
motor = wpilib.Jaguar(3)
encoder = wpilib.Encoder(10,11)

motor1 = wpilib.Jaguar(1)
motor2 = wpilib.CANJaguar(10)

drive = wpilib.RobotDrive(motor1, motor2)
drive.SetSafetyEnabled(False)
drive.SetInvertedMotor( wpilib.RobotDrive.kRearLeftMotor, False )
drive.SetInvertedMotor( wpilib.RobotDrive.kRearRightMotor, True )
spike1 = wpilib.Relay(1) 
spike2 = wpilib.Relay(3)

class MyRobot(wpilib.SimpleRobot):

    def __init__(self):
        wpilib.SimpleRobot.__init__(self)
        encoder.Start()

    def Disabled(self):
        
        print("MyRobot::Disabled()")
    
        while self.IsDisabled():
            wpilib.Wait(0.01)

    def Autonomous(self):
        self.GetWatchdog().SetEnabled(False)
        while self.IsAutonomous() and self.IsEnabled():
            wpilib.Wait(0.01)
            
            
    def OperatorControl(self):
    
        print("MyRobot::OperatorControl()")
        #watch dog time out
        timeOut = 0.25
        dog = self.GetWatchdog()
        dog.SetEnabled(True)
        dog.SetExpiration(timeOut)
        
        timer = wpilib.Timer()
        timer.Start()
        
        on = False

        # main loop
        while self.IsOperatorControl() and self.IsEnabled():
   
            dog.Feed()
            if stick1.GetTrigger():
                motor.Set( 0 )
                motor1.Set( stick1.GetZ() )
                motor2.Set( stick1.GetZ() * -1.0 )
            elif stick2.GetTop():
                motor.Set( 0 )
                drive.ArcadeDrive( stick2 )
                
            else:
                motor.Set( 0 )
                motor1.Set( 0 )
                motor2.Set( 0 )
               
            if timer.HasPeriodPassed( 1.0 ):
                print( "Motor: %f,%f,%f; Encoder: %d" % ( motor.Get(), motor1.Get(), motor2.Get(), encoder.Get() ) )
            
            if stick1.GetRawButton(6): 
                spike1.Set(wpilib.Relay.kForward)
            elif stick1.GetRawButton(7):               
               spike1.Set(wpilib.Relay.kReverse)
            else:
                spike1.Set(wpilib.Relay.kOff)
                
                
                
            if stick1.GetRawButton(10): 
                spike2.Set(wpilib.Relay.kForward)
            elif stick1.GetRawButton(11):               
               spike2.Set(wpilib.Relay.kReverse)
            else:
                spike2.Set(wpilib.Relay.kOff)
            
            wpilib.Wait(0.04)
                
        
def run():
    #rpdb2.start_embedded_debugger()

    '''This function must be present for the robot to start'''
    robot = MyRobot()
    robot.StartCompetition()




