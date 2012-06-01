
try:
    import wpilib
except ImportError:
    import fake_wpilib as wpilib


class RampArm(object):
    '''
        The RampArm is a big weight attached to a motor that we can use
        to lower the ramp so we can go over it. Or do something along 
        those lines. 
        
        Currently, the way the arm works is it is always trying to go
        in one direction or the other. Ideally, we connect this to a
        button, and when the operator presses a button the arm goes down, 
        and when they release it the arm goes back up.
        
        There is a limit switch attached to the jaguar which prevents the 
        motor from retracting past its hard stop.
    '''


    EXTEND_ARM = 1.0
    RETRACT_ARM = -0.5

    def __init__(self, rampArmCAN):
        self.arm = wpilib.CANJaguar(rampArmCAN)
        self.extend = False
    
    def Extend(self):
        '''Extends the arm to move the ramp'''
        self.extend = True
  
    def Update(self):
    
        if self.extend:
            self.arm.Set( RampArm.EXTEND_ARM )
        else:
            self.arm.Set( RampArm.RETRACT_ARM )
            
        self.extend = False
            