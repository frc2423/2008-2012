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
            - Top button does a 'hold'
            - 6 does deployment
            - 7 does retrieval
            
            - 4 & 8 - thump motor down
            - 5 & 9 - thump motor up
            
    Driver station:
        - Inputs 1-6 set vertical position
        - Input 7 determines autonomous behavior??
        - Outputs 1-6 indicate vertical position (LED)
        - Output 7 indicates line tracking (LED)
'''

try:
    import wpilib
except:
    import fake_wpilib as wpilib

import arm
import auto

robot = None


class MyRobot(wpilib.SimpleRobot):

    def __init__(self):
    
        print("MyRobot::__init__()")
    
        # drive motors
        self.l_motor = wpilib.CANJaguar( 23 )
        self.r_motor = wpilib.CANJaguar( 24 )
        
        # set to coast
        self.l_motor.ConfigNeutralMode( wpilib.CANJaguar.kNeutralMode_Coast )
        self.r_motor.ConfigNeutralMode( wpilib.CANJaguar.kNeutralMode_Coast )
 
        self.drive = wpilib.RobotDrive(self.l_motor, self.r_motor)
        
        # other useful things
        self.arm = arm.Arm()
        self.drive_stick = wpilib.Joystick(1)
        self.arm_stick = wpilib.Joystick(2)
        self.ds = wpilib.DriverStation.GetInstance()
        self.auto = auto.Auto()
        
        
    def CheckRestart(self):
        if self.drive_stick.GetRawButton(10):
            raise RuntimeError("Restart")

    def Disabled(self):
    
        # TODO: Set arm positions here? 
    
        print("MyRobot::Disabled()")
        while self.IsDisabled():
            self.CheckRestart()
            wpilib.Wait(0.01)

    def drive_robot_with_joystick(self):
        '''Utility function to allow the user to control the robot with a joystick'''
    
        y = self.drive_stick.GetY()
        x = self.drive_stick.GetX() * 0.7
        
        # By default, enable smoother turning
        if not self.drive_stick.GetTop():
            
            if x >= 0.0:
                x = x * x
            else:
                x = -(x * x)
            
        # Send the control to the motor
        self.drive.ArcadeDrive(y, x, False)
            
    def Autonomous(self):
    
        print("MyRobot::Autonomous()")
    
        self.GetWatchdog().SetEnabled(False)
        
        # keep track of how much time has passed in autonomous mode
        timer = wpilib.Timer()
        timer.Start()
        
        # determine which position we want the arm to go to..
        self.arm.set_vertical_position( 3 )
        self.arm.set_thump_position( .5 )
        
        while self.IsAutonomous() and self.IsEnabled():
            
            # control loops
            self.auto.update_line_tracking(self.ds, timer.Get())
            self.auto.do_control_loop(self.drive, self.arm, timer.Get())
            self.arm.do_control_loop()
            
            wpilib.Wait(0.04)

    def OperatorControl(self):
    
        print("MyRobot::OperatorControl()")
        
        dog = self.GetWatchdog()
        dog.SetEnabled(False)
        dog.SetExpiration(0.25)
        
        holding = False

        while self.IsOperatorControl() and self.IsEnabled():
        
            dog.Feed()
            self.CheckRestart()
            
            #############
            # Decisions #
            #############
            
            # Automated arm height
            for k,v in arm.arm_position_map.items():
                # inputs are inverted
                if not self.ds.GetDigitalIn(k):
                    self.arm.set_vertical_position(v)
            
            # Manual Arm Control
            arm_y = self.arm_stick.GetY()
            
            arm_trigger = self.arm_stick.GetTrigger()
            
            # make sure that if you press the hold button while holding
            # the trigger (manual arm mode), then you should not allow
            # manual control until you let go of the trigger again
            if not holding:
                if arm_trigger:
                    self.arm.manual_vertical_control(self.arm_stick.GetY())
            else:
                if arm_trigger == False:
                    holding = False
            
            if self.arm_stick.GetTop():
                self.arm.set_hold_position()
                holding = True
                
            # Tube Deployment
            if self.arm_stick.GetRawButton(6):
                self.arm.deploy_tube()
            elif self.arm_stick.GetRawButton(7):
                self.arm.retrieve_tube()
            
            # Thump motor position
            # TODO: This is non-optimal. We really want the position set
            # automatically if the user hits a button -- particularly for
            # one of the top sections. How do we do that intuitively?
            #self.arm.set_thump_position( (self.arm_stick.GetZ() + 1.0) / 2.0 )
            
            if self.arm_stick.GetRawButton(4) or self.arm_stick.GetRawButton(8):
                self.arm.manual_thump_control( 1.0 )
            elif self.arm_stick.GetRawButton(5) or self.arm_stick.GetRawButton(9):
                self.arm.manual_thump_control( -1.0 )
            
            #################
            # Control Loops #
            #################
            
            # update line tracking state first (always)
            self.auto.update_line_tracking(self.ds, None)
            
            if self.drive_stick.GetTrigger():
                # Automated Placement
                self.auto.do_control_loop(self.drive, self.arm, None)
                
            else:
                # Driver Control
                self.drive_robot_with_joystick()
            
            self.arm.do_control_loop()
            self.arm.set_arm_indicators(self.ds)
            
            
            
            wpilib.Wait(0.05)

def run():

    # for unit tests.. 
    global robot

    #import rpdb2
    #rpdb2.start_embedded_debugger()

    robot = MyRobot()
    robot.StartCompetition()
