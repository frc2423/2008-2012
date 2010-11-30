import wpilib
from kicker import kicker

lstick = wpilib.Joystick(1)


def CheckRestart():
    '''This restarts the robot'''
    
    if lstick.GetRawButton(10):
        raise RuntimeError("Restart")

class MyRobot(wpilib.SimpleRobot):

    def __init__(self):
        '''Constructor'''
        self.kicker = kicker()

    def Disabled(self):
        while self.IsDisabled():
            CheckRestart()
            wpilib.Wait(0.01)

    def Autonomous(self):
        self.GetWatchdog().SetEnabled(False)
        while self.IsAutonomous() and self.IsEnabled():
            CheckRestart()
            wpilib.Wait(0.01)

    def OperatorControl(self):
        dog = self.GetWatchdog()
        dog.SetEnabled(True)
        dog.SetExpiration(0.25)

        # main loop
        while self.IsOperatorControl() and self.IsEnabled():
            dog.Feed()
            
            CheckRestart()

            # Motor control
            #motor.Set(lstick.GetY())

            # kicker control
            self.kicker.execute()
            
            wpilib.Wait(0.04)

def run():
    robot = MyRobot()
    robot.StartCompetition()




