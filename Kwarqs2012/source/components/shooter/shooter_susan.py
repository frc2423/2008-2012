try:
    import wpilib
except ImportError:
    import fake_wpilib as wpilib



class Susan(object):
    
    global forwards
    forwards = 1
    off = 0
    backwards = -1
    '''mNumber is CANJaguar ID'''
    def __init__(self,susanCAN,susanGyro):
        self.susanMotor = wpilib.CANJaguar(susanCAN)
        self.CurrentAngle = 0
        self.GoalAngle = 0
        self.gyro = wpilib.Gyro(susanGyro)
    
    '''override function when called turn'''
    def turn(self,mNumber):
        self.susanMotor.set( forwards)    
        
    '''set goal angle variable'''
    def SetGoal(self,degrees):
        self.GoalAngle = degrees
    
    '''checks if Goal angle is = to the current angle'''
    def IsReady(self):
        if self.GoalAngle == self.CurrentAngle:
            return True
    '''updates variables'''
    def Update(self):
        self.mNumber.Set
        self.gryo.SetAngle(self.GoalAngle)
        self.susanMotor.set(forwards)