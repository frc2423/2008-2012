
#import rpdb2

try:
    import wpilib
except ImportError:
    import fake_wpilib as wpilib
import robotmanager

#Hardware Initilization Values
rampArmMotorNum = 3

robotManager = robotmanager( rampArmMotorNum)

stick1 = wpilib.Joystick(1)
stick2 = wpilib.Joystick(2)

driveMotor1 = Jaguar(1)
driveMotor2 = Jaguar(2)

# this is the motor attached to the encoder


drive = wpilib.RobotDrive( driveMotor1, driveMotor2)

'''spike1 = wpilib.Relay(1) 
spike2 = wpilib.Relay(3)'''

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
        
        # main loop
        while self.IsOperatorControl() and self.IsEnabled():
            
            dog.Feed()            
            
            drive.ArcadeDrive(stick1)
            
            if stick1.GetTop(): # makes the arm go down --- when button not pressed, the arm makes its ascent back up
                RobotManager.LowerRamp()
          
            if stick1.GetTrigger(): #this is to fire the ball
                RobotManager.ShootIfReady()
            
            if stick1.GetRawButton(4)
                RobotManager.FeedOverride()
            
            ''' 
            this is for the manuel override for the aiming, its sets a state when a button is pressed to initiate it.
            
            if stick2.GetRawButton(7) 
                state = manual 
               
            else state = automatic
            '''
               
               #this prints values so we know what's going on
            if timer.HasPeriodPassed( 1.0 ):
                print( "Motor: %f,%f" % ( driveMotor1.Get(), driveMotor2.Get()  ) )
            
            
            wpilib.Wait(0.04)
                
        
def run():
    #rpdb2.start_embedded_debugger()

    '''This function must be present for the robot to start'''
    robot = MyRobot()
    robot.StartCompetition()




