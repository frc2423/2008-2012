
#import rpdb2
from robot_manager import RobotManager

try:
    import wpilib
except ImportError:
    import fake_wpilib as wpilib







robotManager = RobotManager()

stick1 = wpilib.Joystick(1)
stick2 = wpilib.Joystick(2)
stick3 = wpilib.Joystick(3)

driveMotor1 = wpilib.Jaguar(1)
driveMotor2 = wpilib.Jaguar(2)

drive = wpilib.RobotDrive( driveMotor1, driveMotor2)

driveStation = wpilib.DriveStation.GetInstance()

'''spike1 = wpilib.Relay(1) 
spike2 = wpilib.Relay(3)'''

class MyRobot(wpilib.SimpleRobot):

    def __init__(self):
        wpilib.SimpleRobot.__init__(self)
       
    
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
            
            drive.ArcadeDrive(stick1.GetY(), stick1.GetX())
'''            
            if robotManager.angleState == robotManager.ManualAngle: # my way of manually controlling the ball angle and  the susan
                
                susanMotor.Set( stick2.GetX() )
                angleMotor.Set( stick2.GetY() )
'''           
            
            if stick1.GetTop(): # makes the arm go down --- when button not pressed, the arm makes its ascent back up
                robotManager.LowerRampArm()
            
            if stick1.GetTrigger(): #this is to fire the ball
                robotManager.ShootIfReady()
                          
            if driveStation.getDigitalIn(1): #to manually run the feeder
                robotManager.RunFeederMotor()
            
            if driveStation.getDigitalIn(2): # to manually run the chamber
                robotManager.RunChamberMotor()
            
            if driveStation.getDigitalIn(3): #to manually shoot the ball
                robotManager.SetShooterSpeedManual()
                
            if driveStation.GetDigitalIn(4): #to manually aim both the angle and the position of the lazy susan
                robotManager.SetAngleManual()
            #run chamber motor
            if driveStation.GetDigitalIn(5):
                robotManager.SetSusanManual()
            
            if stick
               #this prints values so we know what's going on
            if stick1.GetRawButton(7):
                print( "Motor: %f,%f" % ( driveMotor1.Get(), driveMotor2.Get()  ) )
            
'''            
            wpilib.Wait(0.04)
                
        
def run(): 
    #rpdb2.start_embedded_debugger()

    '''This function must be present for the robot to start'''
    robot = MyRobot()
    robot.StartCompetition()
    return robot



