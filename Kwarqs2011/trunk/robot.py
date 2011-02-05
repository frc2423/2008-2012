'''
Driver controls:

    Joystick 1 (drive_stick):
        - X/Y drives the robot
        - Trigger causes automated deployment
        - Button 7 deploys minibot
    Joystick 2 (arm_stick):
        - Y does manual arm movement
        - Trigger causes tube deployment
        - Top causes tube retrieval
    Driver station:
        - 6 buttons set vertical position
        - 6 LEDs indicate vertical position
        - 1 LED indicates line tracking
'''

import wpilib
import arm
import auto

# key:   digital input port
# value: arm position
arm_height_map = {
    1: ARM_1,
    2: ARM_2,
    3: ARM_3,
    4: ARM_4,
    5: ARM_5,
    6: ARM_6
}

class MyRobot(wpilib.SimpleRobot):

    def __init__(self):
        self.arm = arm.Arm()
        self.driver_stick = wpilib.Joystick(1)
        self.arm_stick = wpilib.Joystick(2)
        self.ds = wpilib.DriverStation.GetInstance()
        self.auto = auto.Auto()
        self.drive = wpilib.RobotDrive(1, 2)
        
    def CheckRestart():
        if self.driver_stick.GetRawButton(10):
            raise RuntimeError("Restart")

    def Disabled(self):
        while self.IsDisabled():
            self.CheckRestart()
            wpilib.Wait(0.01)

    def drive_robot_with_joystick(self):
        '''Utility function to allow the user to control the robot with a joystick'''
    
        y = self.driver_stick.GetY()
        x = self.driver_stick.GetX() * 0.7
        
        # By default, enable smoother turning
        if not self.driver_stick.GetTop():
            
            if x >= 0.0:
                x = x * x
            else:
                x = -(x * x)
            
        # Send the control to the motor
        self.drive.ArcadeDrive(y, x, False)
            
    def Autonomous(self):
        self.GetWatchdog().SetEnabled(False)
        while self.IsAutonomous() and self.IsEnabled():
            wpilib.Wait(0.04)
            self.auto.do_control_loop(self.drive, self.arm)

    def OperatorControl(self):
        dog = self.GetWatchdog()
        dog.SetEnabled(True)
        dog.SetExpiration(0.25)

        while self.IsOperatorControl() and self.IsEnabled():
            dog.Feed()
            self.CheckRestart()
            
            #############
            # Decisions #
            #############
            
            # Manual Arm Control
            arm_y = self.arm_stick.GetY()
            
            if arm_y > 0.3 or arm_y < -0.3:
                self.arm.manual_vertical_control(arm_y)
            
            # Arm Height
            for k,v in arm_height_map:
                if self.ds.GetDigitalIn(k):
                    self.arm.set_vertical_position(v)
                
            # Tube Deployment
            if self.arm_stick.GetTrigger():
                self.arm.deploy_tube()
            elif self.arm_stick.GetTop():
                self.arm.retrieve_tube()
            
            if self.drive_stick.GetTrigger():
                # Automated Placement
                self.auto.do_control_loop(self.drive, self.arm)
                
            else:
                # Driver Control
                self.drive_robot_with_joystick()
            
            #################
            # Control Loops #
            #################
            
            self.arm.do_control_loop()
            
            wpilib.Wait(0.04)

def run():
    robot = MyRobot()
    robot.StartCompetition()
