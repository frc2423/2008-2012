import wpilib
import kicker

lstick = wpilib.Joystick(1) 
#kicker = kicker.Kicker()

input = wpilib.DigitalInput( 14 )


def CheckRestart():
    if lstick.GetRawButton(10):
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

    def OperatorControl(self):
        dog = self.GetWatchdog()
        dog.SetEnabled(True)
        dog.SetExpiration(0.25)
        
        timer = wpilib.Timer()
        timer.Start()

        while self.IsOperatorControl() and self.IsEnabled():
            dog.Feed()
            CheckRestart()
            
            if timer.Get() >= 1.0:
                if input.Get():
                    print("ON")
                else:
                    print("OFF")
                    
                timer.Reset()
            
            #kicker.kicker_fn(lstick)
            
            wpilib.Wait(0.04)

def run():
    robot = MyRobot()
    robot.StartCompetition()
