
#import rpdb2
from robot_manager import RobotManager

try:
    import wpilib
except ImportError:
    import fake_wpilib as wpilib


from components.chamber import Chamber
from components.feeder import Feeder
from components.shooter import Shooter
from components.shooter.shooter_angle import VerticalAngle
from components.shooter.shooter_susan import Susan
from components.shooter.shooter_wheel import Wheel 
from components.ramp_arm import RampArm

#These are all the numbers that correspond to the specific values for the different motors and other hardware
        
#Relay Inputs
feederRelay = 1
chamberRelay = 2
        
#Digital Channel Inputs
chamberSwitch = 1
topFeedSwitch = 2
botFeedSwitch = 3

#Analog Channel Inputs
bodyGyro = 1
susanGyro = 2
topFeedIRSens = 3
chambIRSens = 4

#CAN Bus IDs
shootWheelCAN1 = 2
shootWheelCAN2 = 3
angleCAN = 4
rampArmCAN = 5
susanCAN = 6

#initialize instances 
chamber = Chamber( chamberRelay, chamberSwitch, chambIRSens)

feeder = Feeder(feederRelay,topFeedSwitch,botFeedSwitch,topFeedIRSens)


wheel = Wheel( shootWheelCAN1, shootWheelCAN2 )
susan = Susan( susanCAN, susanGyro, bodyGyro )
vAngle = VerticalAngle(angleCAN)
shooter = Shooter(vAngle,susan,wheel)


# self.shooter = Shooter(angleCAN, susanCAN, susanGyro, shootWheelCAN1, shootwheelCAN2)

rampArm = RampArm(rampArmCAN)


'''chamber,feeder,wheel,susan,vAngle,shooter,rampArm'''
robotManager = RobotManager(chamber,feeder,rampArm)

stick1 = wpilib.Joystick(1)
stick2 = wpilib.Joystick(2)
stick3 = wpilib.Joystick(3)

driveMotor1 = wpilib.Jaguar(1)
driveMotor2 = wpilib.Jaguar(2)

drive = wpilib.RobotDrive( driveMotor1, driveMotor2)

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
                robotManager.LowerRampArm()
            
            if stick1.GetTrigger(): #this is to fire the ball
                robotManager.ShootIfReady()
                                    
            if driveStation.GetDigitalIn(2): # to manually run the chamber/feeder
                robotManager.ToggelChamberFeederAuto()
               
            if driveStation.GetDigitalIn(3): #to manually shoot the ball
                shooter.ToggleAutoWheel()
                
            if driveStation.GetDigitalIn(4): #to manually aim both the angle 
                shooter.ToggleAutoAngle()

            if driveStation.GetDigitalIn(5): #manual run susan
                shooter.ToggleAutoSusan()
            
            if not shooter.autoAngle:
                vAngle.Set(_translate_z_to_angle_motor_position(stick2).getZ())
                
            if not shooter.autoSusan:
                susan.SetPVbus(stick2.getX())
                
            if not shooter.autoWheel:
                if stick1.getTrigger():
                    ''' makes sure to only spin forwards'''
                    pvBus = (stick1.getZ() + 1)/2 
                    wheel.SetPVBus(pvBus)
                
            if not RobotManager.chamberFeederAuto:
                if stick2.GetRawButton(6):
                    chamber.Run()
                if stick2.GetRawButton(7):
                    feeder.Feed()
            #if stick
            #this prints values so we know what's going on
            if stick1.GetRawButton(7):
                print( "Motor: %f,%f" % ( driveMotor1.Get(), driveMotor2.Get()  ) )
                
            robotManager.Update()
            
            
            wpilib.Wait(0.04)
                
        
def run(): 
    #rpdb2.start_embedded_debugger()

    '''This function must be present for the robot to start'''
    robot = MyRobot()
    robot.StartCompetition()
    return robot



