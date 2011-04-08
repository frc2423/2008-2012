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
            - 10 - Set thump motor position using Z
            
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
from util import *

robot = None

AUTONOMOUS_DISABLED = False

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


    def Disabled(self):
    
        # TODO: Set arm positions here? 
        timer = PrintTimer()
        
        print("MyRobot::Disabled()")
        while self.IsDisabled():
        
            if self.ds.IsFMSAttached() and timer.should_print():
                print("[%10f] Waiting for competition to start" % timer.Get())
        
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
        self.arm.set_vertical_position( 4 )
        #self.arm.set_thump_position( .46 )
        self.arm.set_thump_position( 0.08 )
        
        while self.IsAutonomous() and self.IsEnabled():
            
            # control loops
            if not AUTONOMOUS_DISABLED:
            
                try:
                    self.auto.update_line_tracking(self.ds, timer.Get())
                except:
                    if not self.ds.IsFMSAttached():
                        raise

                try:
                    self.auto.do_control_loop(self.drive, self.arm, timer.Get())
                except:
                    if not self.ds.IsFMSAttached():
                        raise
                    
                try:
                    self.arm.do_control_loop()
                except:
                    if not self.ds.IsFMSAttached():
                        raise
            
            wpilib.Wait(0.05)

    def OperatorControl(self):
    
        print("MyRobot::OperatorControl()")
        
        dog = self.GetWatchdog()
        dog.SetEnabled(False)
        dog.SetExpiration(0.25)
        
        holding = False
        print_timer = PrintTimer()

        while self.IsOperatorControl() and self.IsEnabled():
        
            dog.Feed()
            
            #############
            # Decisions #
            #############
            
            # Automated arm height
            try:
                for k,v in arm.arm_position_map.items():
                    # inputs are inverted
                    if not self.ds.GetDigitalIn(k):
                        self.arm.set_vertical_position(v)
            except:
                if not self.ds.IsFMSAttached():
                    raise
            
            # Manual Arm Control
            arm_y = self.arm_stick.GetY()
            
            arm_trigger = self.arm_stick.GetTrigger()
            
            # make sure that if you press the hold button while holding
            # the trigger (manual arm mode), then you should not allow
            # manual control until you let go of the trigger again
            try:
                if not holding:
                    if arm_trigger:
                        self.arm.manual_vertical_control(self.arm_stick.GetY())
                else:
                    if arm_trigger == False:
                        holding = False
            except:
                if not self.ds.IsFMSAttached():
                    raise
            
            try:
                if self.arm_stick.GetTop():
                    self.arm.set_hold_position()
                    holding = True
            except:
                if not self.ds.IsFMSAttached():
                    raise
                
            # Tube Deployment
            try:
                if self.arm_stick.GetRawButton(6):
                    self.arm.deploy_tube()
                elif self.arm_stick.GetRawButton(7):
                    self.arm.retrieve_tube()
            except:
                if not self.ds.IsFMSAttached():
                    raise
            
            # Thump motor positioning
            try:
                if self.arm_stick.GetRawButton(4) or self.arm_stick.GetRawButton(8):
                    self.arm.manual_thump_control( -1.0 )
                    
                elif self.arm_stick.GetRawButton(5) or self.arm_stick.GetRawButton(9):
                    self.arm.manual_thump_control( 1.0 )
                    
                elif self.arm_stick.GetRawButton(10):
                    self.arm.set_thump_position( (self.arm_stick.GetZ() + 1.0) / 2.0 )
            except:
                if not self.ds.IsFMSAttached():
                    raise
                    
            try:
                if self.drive_stick.GetRawButton(10) and print_timer.should_print():
                    self.arm.print_diagnostics()
            except:
                if not self.ds.IsFMSAttached():
                    raise
            
            #################
            # Control Loops #
            #################
            
            # update line tracking state first (always)
            try:
                self.auto.update_line_tracking(self.ds, None)
            except:
                if not self.ds.IsFMSAttached():
                    raise
            
            try:
                # Driver Control
                self.drive_robot_with_joystick()
            except:
                if not self.ds.IsFMSAttached():
                    raise
            
            try:
                self.arm.do_control_loop()
            except:
                if not self.ds.IsFMSAttached():
                    raise
                
            try:
                self.arm.set_arm_indicators(self.ds)
            except:
                if not self.ds.IsFMSAttached():
                    raise
            
            wpilib.Wait(0.05)

def run():

    # for unit tests.. 
    global robot

    #import rpdb2
    #rpdb2.start_embedded_debugger()

    robot = MyRobot()
    robot.StartCompetition()
