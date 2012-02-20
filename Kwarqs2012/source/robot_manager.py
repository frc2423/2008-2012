try:
    import wpilib
except ImportError:
    import fake_wpilib as wpilib

'''
Calls certain functions based on the current states of each component
'''


'''
Imported components so that instances of each can be made 
'''

from components.chamber import Chamber
from components.feeder import Feeder
from components.shooter import Shooter
from components.shooter.shooter_angle import VerticalAngle
from components.shooter.shooter_susan import Susan
from components.shooter.shooter_wheel import Wheel 
from components.ramp_arm import RampArm




class RobotManager(object):

    
    '''
    description: initializes an instance of RobotManager
    Variables: 
    rampArmMotorNum- PWM connection value of Ramparm motor
    
    self.chamberOveride and self.chamberAuto control the state of the chamber
    self.feederOveride and self.feederAuto control the state of the feeder
    
    '''
    def __init__(self, chamber, feeder, wheel, susan, vAngle, shooter, rampArm):
        
        
        self.chamber = chamber
     
        self.feeder = feeder
        
        self.wheel = wheel
        self.susan = susan
        self.vAngle = vAngle
        self.shooter = shooter
        # self.shooter = Shooter(angleCAN, susanCAN, susanGyro, shootWheelCAN1, shootwheelCAN2)
        
        self.rampArm = rampArm
        
    
    #def SetState(): (manual or auto)
    
    '''
    Calls the shoot function if the bot is ready to shoot
    '''
    def ShootIfReady(self):
        if self.shooter.IsReady() and self.chamber.IsReady():
            self.chamber.Run()
                
    '''
    lowers ramp arm
    '''
    def LowerRampArm(self):
        self.rampArm.LowerRamp()
    '''    
    Releases chamber no matter the state, only to be used when things malfunction
    '''
    def RunChamberMotor(self):
        self.chamber.Run()
        
    def RunFeederMotor(self):
        self.feeder.Feed()
       
    def SetShooterSpeedManual(self, speed):
        self.shooter.SetSpeed(speed)
    
    def SetAngleManual(self, angle):
        self.shooter.SetGoal(angle)
        
    def SetSusanManual(self, speed):
        self.shooter.SetSusanSpeed(speed)
    '''
    Calls functions within the components based on every possible state
    '''    
    
    def Update(self):
        '''
        self.shooter.Update()
        '''
        self.chamber.Update()
        self.feeder.Update()
        self.rampArm.Update()
    
    
        '''
        I need to update the states here in the If statements; or to add them as requirements to the if statements
        ^This is meant to take interstates into consideration    
        I'm having trouble figuring out where interstates are needed- since a lot of things only work if a button is being HELD
        on a joystick
        '''
    
    
        if self.chamber.IsReady() and self.feeder.IsFull():
            self.feeder.Stop()
            self.chamber.Stop()
                
        elif not self.chamber.IsReady() and self.feeder.IsReady():
            self.feeder.FeedOveride()
            self.chamber.Run()
                
        elif not self.feeder.IsFull():
            self.feeder.Feed()
        

        '''    
        elif not self.chamber.IsFull() and ((self.Feeder.BallStates() == 1 or self.Feeder.BallStates() == 2) or self.Feeder.IsFull() ):
            if self.chamber.IsReady != 1:
                self.chamber.Run()
                self.feeder.Feed()
        '''        
