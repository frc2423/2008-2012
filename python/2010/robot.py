import wpilib
from kicker import Kicker

# define needed robot resources here
stick = wpilib.Joystick(1)
my_robot = wpilib.RobotDrive( 1, 2 )
kicker = Kicker()


def CheckRestart():
    '''This function restarts the robot when button 10 is pressed'''
    if stick.GetRawButton(10):
        raise RuntimeError("Restart")
        

class MyRobot(wpilib.SimpleRobot):

    def Disabled(self):
        while self.IsDisabled():
            CheckRestart()
            wpilib.Wait(0.01)

    def Autonomous(self):
        self.GetWatchdog().SetEnabled(False)
        while self.IsAutonomous() and self.IsEnabled():
            CheckRestart()
            wpilib.Wait(0.01)

            
    def drive_robot_with_joystick(self):
        '''Utility function to allow the user to control the robot with a joystick'''
    
        y = stick.GetY()
        x = stick.GetX() * 0.7
        
        # by default, enable smoother turning
        if not stick.GetRawButton( 2 ):
            
            if x >= 0.0:
                x = x * x
            else:
                x = -(x * x)
    
        # limit going backwards so we can pull the ball with us more easily
        if y > 0:
            y = y * 0.6
            
        # send the control to the motor
        my_robot.ArcadeDrive( y, x, False )
    
            
    def OperatorControl(self):
        dog = self.GetWatchdog()
        dog.SetEnabled(True)
        dog.SetExpiration(0.25)

        # main loop
        while self.IsOperatorControl() and self.IsEnabled():
        
            # always feed the watchdog!
            dog.Feed()
            
            # check to see if the user has indicated that they want a restart
            CheckRestart()

            # tell the motors to do something
            self.drive_robot_with_joystick()

            # kicker control
            if stick.GetTrigger():
                kicker.kick()
            
            # execute anything needed to make the kicker work
            kicker.do_actions()
            
            wpilib.Wait(0.04)
            
        # always disable the motors when exiting user control
        my_robot.ArcadeDrive( 0.0, 0.0 )

        
def run():
    '''This function must be present for the robot to start'''
    robot = MyRobot()
    robot.StartCompetition()




