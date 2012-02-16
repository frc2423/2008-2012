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
        self.chamberOveride = 0
        self.feederOveride = 0
    #def SetState(): (manual or auto)
    
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
    

    #Feed balls INTO the chamber manually with this function
    def ChamberOverride(self):
        self.chamber.Feed()
        #need a state here so I can turn this motor off
        
    def FeedOverride(self):
        self.feeder.FeedOveride()
    '''
    Calls functions within the components based on every possible state
    '''    
    def Update(self):
        self.Shooter.Update()
        self.chamber.Update()
        self.feeder.Update()
        self.Ramp Arm.Update()
    
    
    '''
    I need to update the states here in the If statements; or to add them as requirements to the if statements
    ^This is meant to take interstates into consideration    
    I'm having trouble figuring out where interstates are needed- since a lot of things only work if a button is being HELD
        on a joystick
    '''
    
    
        if chamber.IsReady() and feeder.IsFull():
            self.feeder.Stop()
            #self.chamber.Stop()
            
        elif not chamber.IsReady() and feeder.IsReady():
            self.feeder.FeedOveride()
            #self.chamber.Feed()
            
        elif not feeder.IsFull():
            self.feeder.Feed()
        
        elif not chamber.IsFull() and ((Feeder.BallStates() == 1 or Feeder.BallStates() == 2) or feeder.IsFull() ):
            if chamber.IsReady != 1:
                self.chamber.Feed()
                self.feeder.Feed()
            
