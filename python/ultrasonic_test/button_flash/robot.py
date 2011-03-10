
#import rpdb2

import wpilib

stick = wpilib.Joystick(1)





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

    def OperatorControl(self):
        dog = self.GetWatchdog()
        dog.SetEnabled(True)
        dog.SetExpiration(0.25)
        
        ds = wpilib.DriverStation.GetInstance()
        
        # main loop
        while self.IsOperatorControl() and self.IsEnabled():
        
            # always feed the watchdog!
            dog.Feed()
            
            # check to see if the user has indicated that they want a restart
            CheckRestart()
            
            on = stick.GetTrigger()
            
            for i in range(1,9):
                ds.SetDigitalOut(i,on)
    
            wpilib.Wait(0.04)

        
def run():
    #rpdb2.start_embedded_debugger()

    '''This function must be present for the robot to start'''
    robot = MyRobot()
    robot.StartCompetition()




