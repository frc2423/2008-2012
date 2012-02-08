'''
    electrical_test 
    
    This program is for the Kwarqs 2012 robot. It is intended to be the 
    most basic program possible to exercise all of the electrical
    functionality of the robot. By having something simple that *just works*,
    we can test the robot when it is having problems and determine
    if the problems are mechanical, electrical, or program-related. 

'''


import wpilib

# our custom python module for tracking baskets
from basket_tracker import BasketTracker

# TODO: Give these better names to reflect what they do
# TODO: Add sensors as needed
# TODO: Organize the motors and sensors better

# Joysticks
stick1 = Joystick(1)
stick2 = Joystick(2)

# PWM motor controllers (drive motors)
motor_pwm1 = wpilib.Jaguar(1)
motor_pwm2 = wpilib.Jaguar(2)

# CAN motor controllers
motor_can2 = wpilib.CANJaguar(2)
motor_can3 = wpilib.CANJaguar(3)
motor_can4 = wpilib.CANJaguar(4)
motor_can5 = wpilib.CANJaguar(5)
motor_can6 = wpilib.CANJaguar(6)

# relays
relay1 = wpilib.Relay(1)
relay2 = wpilib.Relay(2)
relay3 = wpilib.Relay(3)
relay4 = wpilib.Relay(4)

# vision basket tracking code
tracker = BasketTracker()


class MyRobot(wpilib.SimpleRobot):

    def __init__(self):
        wpilib.SimpleRobot.__init__(self)

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
            
            # TODO: Add useful motor controls here
            
            motor_pwm1.Set( stick1.GetY() )
            motor_pwm2.Set( stick1.GetY() )
            
            motor_can2.Set( stick1.GetY() )
            motor_can3.Set( stick1.GetY() )
            motor_can4.Set( stick1.GetY() )
            motor_can5.Set( stick1.GetY() )
            motor_can6.Set( stick1.GetY() )
            
            if stick1.GetTrigger():
                relay1.Set( wpilib.Relay.kForward )
                relay2.Set( wpilib.Relay.kForward )
                relay3.Set( wpilib.Relay.kForward )
                relay4.Set( wpilib.Relay.kForward )
            
            elif stick1.GetTop():
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




