
#import rpdb2
from robot_manager import RobotManager

try:
    import wpilib
except ImportError:
    import fake_wpilib as wpilib
    
from util import PrintTimer


from components.chamber import Chamber
from components.feeder import Feeder
from components.shooter import Shooter
from components.shooter.shooter_angle import VerticalAngle
from components.shooter.shooter_susan import Susan
from components.shooter.shooter_wheel import Wheel 
from components.ramp_arm import RampArm

#These are all the numbers that correspond to the specific values for the different motors and other hardware
        
#Relay Inputs
chamberRelay = 2

#jaguar Inputs
feederJag = 3
        
#Digital Channel Inputs
botFeedSwitch = 1
wheelEncoder = (10,11)

#Analog Channel Inputs
bodyGyro = 1
susanGyro = 2
topFeedIRSens = 5
chambIRSens = 6

#CAN Bus IDs
shootWheelCAN1 = 2
shootWheelCAN2 = 3
angleCAN = 4
rampArmCAN = 5
susanCAN = 6

#initialize instances 
chamber = Chamber( chamberRelay, chambIRSens)

feeder = Feeder(feederJag, botFeedSwitch, topFeedIRSens)


wheel = Wheel( shootWheelCAN1, shootWheelCAN2, wheelEncoder )
susan = Susan( susanCAN, susanGyro, bodyGyro )
vAngle = VerticalAngle(angleCAN)
shooter = Shooter(vAngle,susan,wheel)


rampArm = RampArm(rampArmCAN)


'''chamber,feeder, rampArm,wheel,susan,vAngle,shooter,'''
robotManager = RobotManager(chamber, feeder, wheel, susan, vAngle, shooter)

stick1 = wpilib.Joystick(1)
stick2 = wpilib.Joystick(2)
stick3 = wpilib.Joystick(3)

r_driveMotor = wpilib.Jaguar(1)
l_driveMotor = wpilib.Jaguar(2)

drive = wpilib.RobotDrive( l_driveMotor, r_driveMotor )
drive.SetSafetyEnabled( False )

driveStation = wpilib.DriverStation.GetInstance()

ANGLE_MOTOR_MIN_POSITION = 0
ANGLE_MOTOR_MAX_POSITION = 1.0

def _translate_z_to_angle_motor_position( z ):

    # P = Xmax - (Ymax - Y)( (Xmax - Xmin) / (Ymax - Ymin) )
    value = ANGLE_MOTOR_MAX_POSITION - ((1 - z)*( (ANGLE_MOTOR_MAX_POSITION - ANGLE_MOTOR_MIN_POSITION) / (1.0-0) ) )
    
    if value > ANGLE_MOTOR_MAX_POSITION:
        return ANGLE_MOTOR_MAX_POSITION
    elif value < ANGLE_MOTOR_MIN_POSITION:
        return ANGLE_MOTOR_MIN_POSITION
    return value

class MyRobot(wpilib.SimpleRobot):

    def __init__(self):
        wpilib.SimpleRobot.__init__(self)
        self.print_timer = PrintTimer()
                
    
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
        #watch dog time out
        timeOut = 0.25
        dog = self.GetWatchdog()
        dog.SetEnabled(True)
        dog.SetExpiration(timeOut)
        
        timer = wpilib.Timer()
        timer.Start()
        
        # main loop
        while self.IsOperatorControl() and self.IsEnabled():
            
            dog.Feed()            
            
            drive.ArcadeDrive(stick1.GetY(), stick1.GetX())
            '''            
            if robotManager.angleState == robotManager.ManualAngle: # my way of manually controlling the ball angle and  the susan
                
                susanMotor.Set( stick2.GetX() )
                angleMotor.Set( stick2.GetY() )
            '''           
            
            if stick1.GetTop(): # makes the arm go down --- when button not pressed, the arm makes its ascent back up
                rampArm.Lower()
            
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
            
            rampArm.Update()
            robotManager.Update()
            
    
            wpilib.Wait(0.04)
                
        
def run(): 
    #rpdb2.start_embedded_debugger()

    '''This function must be present for the robot to start'''
    robot = MyRobot()
    robot.StartCompetition()
    return robot



