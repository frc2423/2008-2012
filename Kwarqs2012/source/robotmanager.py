try:
    import wpilib
except ImportError:
    import fake_wpilib as wpilib

'''
Calls certain functions based on the current states of each component
'''

from components import chamber
from components import feeder
from components import shooter
from components import rampArm



class RobotManager(Object):
    
    '''
    description: initializes an instance of RobotManager
    Variables: 
    rampArmMotorNum- PWM connection value of Ramparm motor

    '''
    def __init__(self, rampArmMotorNum):
        self.chamber = Chamber()
        self.feeder = Feeder()
        self.shooter = Shooter()
        self.rampArm = RampArm(rampArmMotorNum)
    
    '''
    Calls the shoot function if the bot is ready to shoot
    '''
    def ShootIfReady(self):
        if shooter.IsReady() and chamber.IsReady():
            self.chamber.Release()
    
    '''
    Releases chamber no matter the state, only to be used when things malfunction
    '''
    def ShootOverride(self):
        self.chamber.Release()
        
    '''
    Calls functions within the components based on every possible state
    '''    
    def Update(self):
        if chamber.IsFull() and feeder.IsFull():
            self.feeder.Stop()
            #chamber.Stop()
            
        elif not chamber.IsFull() and feeder.IsReady():
            self.feeder.FeedOveride()
            #chamber.Feed()
            
        elif not feeder.IsFull():
            self.feeder.Feed()
            
        self.Shooter.Update()
        self.chamber.Update()
        self.feeder.Update()
        self.Ramp Arm.Update()
        