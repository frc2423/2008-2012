
#import rpdb2

import wpilib

stick1 = wpilib.Joystick(1)
stick2 = wpilib.Joystick(2)

motor = wpilib.Jaguar(3)
encoder = wpilib.Encoder(10,11)


class MyRobot(wpilib.SimpleRobot):

    def __init__(self):
        pass

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
    
        dog = self.GetWatchdog()
        dog.SetEnabled(True)
        dog.SetExpiration(0.25)
        
        timer = wpilib.Timer()
        timer.Start()
        
        on = False

        # main loop
        while self.IsOperatorControl() and self.IsEnabled():
   
            dog.Feed()
            motor.Set( stick1.GetY() )
               
            if timer.HasPeriodPassed( 1.0 ):
                print( "Motor: %f; Encoder: %d" % ( motor.Get(), encoder.Get() ) )
            
            wpilib.Wait(0.04)

        
def run():
    #rpdb2.start_embedded_debugger()

    '''This function must be present for the robot to start'''
    robot = MyRobot()
    robot.StartCompetition()




