'''
    Kwarqs 2012 Robotics code
    
        This code is awesome, and makes our robot do cool stuff. :)
    
    Students:
        Ben
        Dylan
        Ghi
        Jeff
        Scudder
    
    Mentors:
        Dustin Spicuzza
        Sam Rosenblum
        Youssef Barhomi
        
'''

try:
    import wpilib
except ImportError:
    import fake_wpilib as wpilib
   
from autonomous import AutonomousModeManager
from controls import *
from operator_leds import OperatorLEDs
from robot_widget import RobotWidget
from util import ConsoleLED, PrintTimer


#
# Import all components here
#

from components import RobotManager, RampArm
from components.ball_handler import BallHandler, Chamber, Feeder
from components.shooter import Camera, Susan, Wheel



#
# These are all the ports and identifiers needed to initialize various
# hardware components needed by the robot
#
        
#Relay Inputs
#chamberRelay    = 2
cameraRelay     = 3

# PWM Jaguars
l_motor_pwm_ch  = 2
r_motor_pwm_ch  = 1
feeder_pwm_ch   = 3
camera_servo_ch = 5
        
#Digital Channel Inputs
botFeedSwitch   = 1
wheelEncoder    = (10,11)

#Analog Channel Inputs
bodyGyro        = 1
susanGyro       = 2
topFeedIRSens   = 5
chambIRSens     = 6

#CAN Bus IDs
shootWheelCAN1  = 2
shootWheelCAN2  = 3
chamberCAN      = 4
rampArmCAN      = 5
susanCAN        = 6

# console LED
consoleLedPort  = 10

#
# Create instances of all components here
#

# operator interface

consoleLed      = ConsoleLED(consoleLedPort)
operatorLeds    = OperatorLEDs()
left_leds       = operatorLeds.GetDigitGroup(1)
right_leds      = operatorLeds.GetDigitGroup(0)
widget          = RobotWidget('Widget')

# control components

l_driveMotor    = wpilib.Jaguar(l_motor_pwm_ch)
r_driveMotor    = wpilib.Jaguar(r_motor_pwm_ch)

drive           = wpilib.RobotDrive(l_driveMotor, r_driveMotor)

chamber         = Chamber(chamberCAN, chambIRSens)
feeder          = Feeder(feeder_pwm_ch, botFeedSwitch, topFeedIRSens)
ballHandler     = BallHandler(chamber, feeder, widget)

camera          = Camera(camera_servo_ch, cameraRelay)
susan           = Susan(susanCAN, susanGyro, bodyGyro)
wheel           = Wheel(shootWheelCAN1, shootWheelCAN2, wheelEncoder, right_leds)

rampArm         = RampArm(rampArmCAN)

robotManager    = RobotManager(ballHandler, camera, wheel, susan, consoleLed)



# TODO: Figure out why this thing is broken.. 
drive.SetSafetyEnabled( False )




class MyRobot(wpilib.SimpleRobot):
    '''
        This is the main container of robot code, and controls the high-level 
        behavior of the robot during the various phases of robot operation.
                    
        Robot actions are run in two phases: 
        
            First, a decision phase. This is where simple sensors should be
            read and decisions about "what" is going to happen are made here.
            Things like feeding a ball in, turning wheels, automated camera 
            stuff... etc.
            
            Components should *not* expect to be always called in the decision
            phase. If they are not called, they should default to doing
            nothing differently, which will prevent unexpected behaviors in 
            autonomous mode, or especially on the transitions between various
            modes.
            
            Second, an action phase. This is where the decisions are executed,
            and values are sent to motors, etc. All components should expect
            their 'Update' function to be called during the update phase.
    '''

    def __init__(self):
        wpilib.SimpleRobot.__init__(self)
        
        self.ds = wpilib.DriverStation.GetInstance()
        self.print_timer = PrintTimer()
        
        self.autonomous_manager = AutonomousModeManager(drive, rampArm, 
                                                        ballHandler, robotManager)
    
    def Disabled(self):
        
        print("MyRobot::Disabled()")
    
        while self.IsDisabled():
            if self.ds.IsNewControlData():
                operatorLeds.InDisabled()
                
            wpilib.Wait(0.01)

    def Autonomous(self):
        '''
            This function runs in autonomous mode. We don't want to put any
            logic here however, look in the 'autonomous' folder for the 
            autonomous mode manager, which will run a user-selected autonomous
            mode.
        '''
        
        print("MyRobot::Autonomous()")
        
        # don't risk the watchdog, hopefully we do everything right here :)
        self.GetWatchdog().SetEnabled(False)
        
        # keep track of how much time has passed in autonomous mode
        timer = wpilib.Timer()
        timer.Start()
        
        try:
            self.autonomous.OnAutonomousEnable()
        except:
            if not self.ds.IsFMSAttached():
                raise
        
        #
        # Autonomous control loop
        #
        
        while self.IsAutonomous() and self.IsEnabled():
 
            try:            
                self.autonomous.Update( timer.Get() )
            except:
                if not self.ds.IsFMSAttached():
                    raise
             
            wpilib.Wait(0.01)
            
        try:
            self.autonomous.OnAutonomousDisable()
        except:
             if not self.ds.IsFMSAttached():
                raise
                
    
    def drive_robot_with_joystick(self):
        '''Utility function to allow the user to control the robot with a joystick
        
        -> From 2011 code.. not currently used, do we want to use this? 
        
        '''
    
        y = self.drive_stick.GetY()
        x = self.drive_stick.GetX() * 0.7
        
        # By default, enable smoother turning
        if not self.drive_stick.GetTop():
            
            if x >= 0.0:
                x = x * x
            else:
                x = -(x * x)
            
        # Send the control to the motor
        self.drive.ArcadeDrive(y, x, self.drive_stick.GetTrigger())    
    
     
    def OperatorControl(self):
        '''
            This function gets called when the operator is controlling the
            robot via joysticks. 
            
            Very limited logic should be placed here -- any interacting with
            the actual motors and sensors should be done at a lower level.
            
            The logic that *should* be here is decisions based on user input.
            So if that logic gets too complex, that means we're probably
            making it too hard for an operator to reliably control the robot.
        '''
    
    
        print("MyRobot::OperatorControl()")
        
        # setup the watchdog
        watchdogTimeout = 0.25
        dog = self.GetWatchdog()
        dog.SetEnabled(True)
        dog.SetExpiration(watchdogTimeout)
        
        # main loop
        while self.IsOperatorControl() and self.IsEnabled():
            
            dog.Feed()
            
            left_led_value = None
            
            drive.ArcadeDrive(GetJoystickAxis(DRIVE_Y_AXIS), GetJoystickAxis(DRIVE_X_AXIS))
            
            
            ###############################################
            # RampArm
            ###############################################
    
            try:
                if StickButtonOn( RAMP_ARM_DOWN_BUTTON ):
                    rampArm.Extend()
            except:
                if not self.ds.IsFMSAttached():
                    raise
            
           
            ###############################################
            # Manual Shooter
            ###############################################
            
            # store this, because we also pass it to robot 
            # manager in case the user doesn't have the switch
            # flipped when the fire button is pressed
            shooter_z = GetJoystickAxis( SHOOTER_WHEEL_AXIS )
            #shooter_z = ((shooter_z * -1.0) + 1.0) / 2.0
            shooter_z = ((shooter_z * -1.0) + 1.0) * 50.0
            
            try:
                if SwitchOn( MANUAL_SHOOTER_WHEEL_ENABLE_SWITCH ):
                    #wheel.SetVBus( shooter_z )
                    wheel.SetAutoSpeed( shooter_z )
                    left_led_value = shooter_z
            except:
                if not self.ds.IsFMSAttached():
                    raise
            
            
            ###############################################
            # Camera
            ###############################################            
            
            try:
                if StickButtonOn( MANUAL_CAMERA_ENABLE_BUTTON ):
                
                    # TODO: not sure what the appropriate angles are for this
                    angle = GetJoystickAxis( MANUAL_CAMERA_AXIS )
                    angle = ((angle * -1.0) + 1.0) * 90.0
                
                    camera.SetAngle( angle )
                
            except:
                if not self.ds.IsFMSAttached():
                    raise    
                    
           
            ###############################################
            # Susan
            ###############################################
           
            try:
                if StickButtonOn( SUSAN_TURN_BUTTON ):
                    susan.SetVBus( GetJoystickAxis( SUSAN_TURN_AXIS ) )
                
            except:
                if not self.ds.IsFMSAttached():
                    raise
            
            
            ###############################################
            # Ball handler
            ###############################################
           
            #
            # Enable automated behaviors if desired
            #
           
            try:
                if SwitchOn( AUTO_BALL_HANDLER_SWITCH ):
                    ballHandler.AutoFeed()
            
                if SwitchOn( CONTINUOUS_BALL_HANDLER_SWITCH ):
                    ballHandler.ContinuousFeed()
                
            except:
                if not self.ds.IsFMSAttached():
                    raise

            #
            # Always allow manual control
            #
             
            try:
                if StickButtonOn( BALL_HANDLER_MANUAL_CHAMBER_UP_BUTTON ):
                    chamber.Run()
                    
                elif StickButtonOn( BALL_HANDLER_MANUAL_CHAMBER_DOWN_BUTTON ):
                    chamber.Remove()
                
            
                if StickButtonOn( BALL_HANDLER_MANUAL_FEEDER_FEED_BUTTON ):
                    feeder.Feed()
                    
                elif StickButtonOn( BALL_HANDLER_MANUAL_FEEDER_EXPEL_BUTTON ):
                    feeder.Expel()
                    
            except:
                if not self.ds.IsFMSAttached():
                    raise
           
           
            ###############################################
            # Robot Manager
            ###############################################
           
            try:
                if SwitchOn( AUTO_SUSAN_SWITCH ):
                    robotManager.EnableAutoSusan()     
            except:
                if not self.ds.IsFMSAttached():
                    raise
                    
            try:
                # must always call either EnableAutoDistance
                # or SetUserWheelSpeed on the robot manager
                if SwitchOn( AUTO_DISTANCE_SWITCH ):
                    robotManager.EnableAutoDistance()
                else:
                    robotManager.SetUserWheelSpeed( shooter_z )
                    left_led_value = shooter_z
            except:
                if not self.ds.IsFMSAttached():
                    raise
           
            try:
                if StickButtonOn( SHOOT_BALL_BUTTON ):
                    robotManager.ShootBall()
            except:
                if not self.ds.IsFMSAttached():
                    raise
                    
                    
            ###############################################
            # LEDs
            ###############################################
            
            try:
                left_leds.Set( left_led_value )
            except:
                if not self.ds.IsFMSAttached():
                    raise
           
            ###############################################
            # Debug support
            ###############################################
           
            #
            # Don't enable debug output on the field, you can't
            # see it there anyways, and we might cause an exception
            #
            
            if not self.ds.IsFMSAttached():
            
                if StickButtonOn( DEBUG_BUTTON1 ) or StickButtonOn( DEBUG_BUTTON2 ):
                    self._debug_print()
            
    
            ###############################################
            # Update phase
            ###############################################
    
            self._Update()
            
            # wait for more user input
            wpilib.Wait(0.02)
            
            
    def _Update(self):
        '''
            Run the update functions for all components. Called from
            OperatorControl and Autonomous modes
            
            These should be done in a particular order! We want to call 
            the high level abstractions first, followed by the low level
            classes last.
            
            Update functions are ONLY called from here, so we don't 
            accidentally call it twice from somewhere (which could
            cause unexpected results!!)
        '''
        
        # high level stuff first
        
        try:
            robotManager.Update()
        except:
            if not self.ds.IsFMSAttached():
                raise
                
        try:
            ballHandler.Update()
        except:
            if not self.ds.IsFMSAttached():
                raise
        
        
        # low level stuff last
        
        try:
            camera.Update()
        except:
            if not self.ds.IsFMSAttached():
                raise
        
        try:
            susan.Update()
        except:
            if not self.ds.IsFMSAttached():
                raise
        
        try:
            wheel.Update()
        except:
            if not self.ds.IsFMSAttached():
                raise
        
        try:
            chamber.Update()
        except:
            if not self.ds.IsFMSAttached():
                raise
                
        try:
            feeder.Update()
        except:
            if not self.ds.IsFMSAttached():
                raise

        try:
            rampArm.Update()    
        except:
            if not self.ds.IsFMSAttached():
                raise
        
        #
        # These two only get called when there is new data received
        # from the driver station
        #
        
        if self.ds.IsNewControlData():
        
            try:
                consoleLed.Update()
            except:
                if not self.ds.IsFMSAttached():
                    raise
    
            try:
                operatorLeds.Update()
            except:
                if not self.ds.IsFMSAttached():
                    raise
                    
    def _debug_print(self):
        '''Note: don't call this if the FMS is attached'''
    
        if self.print_timer.should_print('debug'):
            robotManager.Print()
            ballHandler.Print()
            camera.Print()
            susan.Print()
            wheel.Print()
            print('')
        
def run(): 
    #rpdb2.start_embedded_debugger()

    '''This function must be present for the robot to start'''
    robot = MyRobot()
    robot.StartCompetition()
    return robot



