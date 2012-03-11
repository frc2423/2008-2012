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
   

from robot_manager import RobotManager   
from util import PrintTimer

#
# Import all components here
#

from components.chamber import Chamber
from components.feeder import Feeder
from components.shooter import Shooter
from components.shooter.shooter_susan import Susan
from components.shooter.shooter_wheel import Wheel 
from components.ramp_arm import RampArm

#
# These are all the ports and identifiers needed to initialize various
# hardware components needed by the robot
#
        
#Relay Inputs
chamberRelay = 2

# PWM Jaguars
l_motor_pwm_ch  = 2
r_motor_pwm_ch  = 1
feeder_pwm_ch   = 3
        
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
angleCAN        = 4
rampArmCAN      = 5
susanCAN        = 6

# driver station inputs

# driver station outputs


#
# Create instances of all components here
#

chamber = Chamber(chamberRelay, chambIRSens)

feeder = Feeder(feeder_pwm_ch, botFeedSwitch, topFeedIRSens)

wheel = Wheel(shootWheelCAN1, shootWheelCAN2, wheelEncoder)
susan = Susan(susanCAN, susanGyro, bodyGyro)
shooter = Shooter(vAngle, susan, wheel)

rampArm = RampArm(rampArmCAN)

robotManager = RobotManager(chamber, feeder, wheel, susan, shooter)

l_driveMotor = wpilib.Jaguar(l_motor_pwm_ch)
r_driveMotor = wpilib.Jaguar(l_motor_pwm_ch)


drive = wpilib.RobotDrive(l_driveMotor, r_driveMotor)

# TODO: Figure out why this thing is broken.. 
drive.SetSafetyEnabled( False )

# joysticks
stick1 = wpilib.Joystick(1)
stick2 = wpilib.Joystick(2)



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
        
        self.autonomous_manager = AutonomousManager(ds, drive, rampArm, ballHandler, shooter, robotManager)
    
    def Disabled(self):
        
        print("MyRobot::Disabled()")
    
        while self.IsDisabled():
            if self.ds.IsNewControlData():
                operator_leds.InDisabled()
                
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
            
            #
            # TODO: Fix controls to match descriptions in controls.txt ...
            #
            
            
            drive.ArcadeDrive(stick1.GetY(), stick1.GetX())
            '''            
            if robotManager.angleState == robotManager.ManualAngle: # my way of manually controlling the ball angle and  the susan
                
                susanMotor.Set( stick2.GetX() )
                angleMotor.Set( stick2.GetY() )
            '''           
            
            
            # makes the arm go down --- when button not pressed, the arm makes its ascent back up
            if stick1.GetTop(): 
                rampArm.Extend()
            
            # if stick1.GetTrigger(): #this is to fire the ball
                # robotManager.ShootIfReady()
                                    
            if driveStation.GetDigitalIn(2): # to manually run the chamber/feeder
                robotManager.DisableAutoFeeder()
                
                if stick2.GetRawButton(6):
                    feeder.Feed()
                elif stick2.GetRawButton(7):
                    feeder.Stop()
                    
            # if driveStation.GetDigitalIn(3): #to manually shoot the ball
                # if shooter.autoWheel:
                    # shooter.ToggleAutoWheel()
            # else:
                # if not shooter.autoWheel:
                    # shooter.ToggleAutoWheel()
                    
            # if driveStation.GetDigitalIn(4): #to manually aim both the angle 
                # if shooter.autoAngle:
                    # shooter.ToggleAutoAngle()
            # else:
                # if not shooter.autoAngle:
                    # shooter.ToggleAutoAngle()
                    
            if driveStation.GetDigitalIn(5): #manually run susan
                susan.SetVBus(stick2.GetX())
 
            
                    
            # if not shooter.autoAngle:
                # vAngle.Set(_translate_z_to_angle_motor_position(stick2.GetZ())
                
            # if not shooter.autoWheel:
                # if stick1.GetTrigger():
                    # ''' makes sure to only spin forwards'''
                    # vBus = (stick1.GetZ() + 1)/2 
                    # wheel.SetVBus(vBus)
                
            # if not RobotManager.chamberFeederAuto:
                # if stick2.GetRawButton(6):
                    # chamber.Run()
                # if stick2.GetRawButton(7):
                    # feeder.Feed()
            #if stick
            #this prints values so we know what's going on
            if stick1.GetRawButton(8) and self.print_timer.should_print():
                feeder.Print()
                print("")
                chamber.Print()
    
    
            # update phase
            self._Update()
            
            # wait for more user input
            wpilib.Wait(0.04)
            
            
    def _Update(self):
        '''
            Run the update functions for all components. Called from
            OperatorControl and Autonomous modes
        '''
        
        try:
            rampArm.Update()    
        except:
            if not self.ds.IsFMSAttached():
                raise
        
        try:
            robotManager.Update()
        except:
            if not self.ds.IsFMSAttached():
                raise
    
            
                
        
def run(): 
    #rpdb2.start_embedded_debugger()

    '''This function must be present for the robot to start'''
    robot = MyRobot()
    robot.StartCompetition()
    return robot



