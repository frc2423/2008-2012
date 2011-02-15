'''
Driver controls:

    Joystick 1 (drive_stick):
        - X/Y drives the robot
        - Trigger causes automated deployment
        - Top button lets you turn faster
        - Button 7 deploys minibot
        
    Joystick 2 (arm_stick):
        - Y does manual arm movement
        
        Original proposal:
            - Trigger causes tube deployment
            - Top causes tube retrieval
            
        Current implementation:
            - Holding trigger lets Y do manual arm movement
            - 6 does deployment
            - 7 does retrieval
            
    Driver station:
        - Inputs 1-6 set vertical position
        - Input 7 determines autonomous behavior??
        - Outputs 1-6 indicate vertical position (LED)
        - Output 7 indicates line tracking (LED)
'''

import wpilib
import arm
import auto



class MyRobot(wpilib.SimpleRobot):

    def __init__(self):
    
        # drive motors
        self.l_motor = wpilib.CANJaguar( 24 )
        self.r_motor = wpilib.CANJaguar( 23 )
        
        self.drive = wpilib.RobotDrive(self.l_motor, self.r_motor)
        
        # other useful things
        self.arm = arm.Arm()
        self.driver_stick = wpilib.Joystick(1)
        self.arm_stick = wpilib.Joystick(2)
        self.ds = wpilib.DriverStation.GetInstance()
        self.auto = auto.Auto()
        
        
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
        
        # determine which position we want the arm to go to..
        if self.ds.GetDigitalIn(7):
            self.arm.set_vertical_position( arm.ARM_1 )
        else:
            self.arm.set_vertical_position( arm.ARM_2 )
        
        while self.IsAutonomous() and self.IsEnabled():
            
            # control loops
            self.auto.update_line_tracking(self.ds)
            self.auto.do_control_loop(self.drive, self.arm)
            self.arm.do_control_loop()
            
            wpilib.Wait(0.04)

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
            
            # Arm Height
            for k,v in arm.arm_height_map:
                if self.ds.GetDigitalIn(k):
                    self.arm.set_vertical_position(v)
            
            # Manual Arm Control
            arm_y = self.arm_stick.GetY()
            
            if self.arm_stick.GetTrigger():
                self.arm.manual_vertical_control(arm_y)
                
            # Tube Deployment
            if self.arm_stick.GetRawButton(6):
                self.arm.deploy_tube()
            elif self.arm_stick.GetRawButton(7):
                self.arm.retrieve_tube()
            
            
            #################
            # Control Loops #
            #################
            
            # update line tracking state first (always)
            self.auto.update_line_tracking(self.ds)
            
            if self.drive_stick.GetTrigger():
                # Automated Placement
                self.auto.do_control_loop(self.drive, self.arm)
                
            else:
                # Driver Control
                self.drive_robot_with_joystick()
            
            self.arm.do_control_loop()
            self.arm.set_arm_indicators(self.ds)
            
            
            
            wpilib.Wait(0.04)

def run():
    robot = MyRobot()
    robot.StartCompetition()
