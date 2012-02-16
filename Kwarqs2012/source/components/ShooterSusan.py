try:
    import wpilib
except ImportError:
    import fake_wpilib as wpilib



def Susan(object):
    
    '''mNumber is CANJaguar ID'''
    def __init__(self,mNumber,gyro):
       self.mNumber = CANJaguar
        self.CurrentAngle = 0
        self.GoalAngle = 0
        self.gyro = Gyro
    
    def turn(self,mNumber):
        forwards = 1
        off = 0
        backwards = -1
    
        motor.set(forwards)
        
    def SetGoal(degrees):
        self.GoalAngle = degrees
    
    
    def IsReady(self):
        if self.GoalAngle == self.CurrentAngle:
            return True
    
    def Update(self):
        self.mNumber.Set
        self.gryo.SetAngle(self.GoalAngle)
        motor.set(forwards)