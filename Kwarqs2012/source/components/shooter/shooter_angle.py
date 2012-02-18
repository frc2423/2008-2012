try:
    import wpilib
except ImportError:
    import fake_wpilib as wpilib


class VerticalAngle(object):
    
    
    
    def __init__(self, angleCAN, anglePot):
        self.verticalGoalAngle = 0
        self.verticalCurrentAngle = 0 
        
        self.motor = wpilib.CANJaguar(angleCAN)
        self.motor.SetPositionReference(wpilib.CANJaguar.kPosRef_Potentiometer)
    
    #set angle to angle given in goalAngle
    def SetGoal(self,degrees):
        self.goalVeticalAngle = degrees
      
    
    #check if GoalAngle = CurrentAngle
    def IsReady(self):
        if self.verticalGoalAngle == self.verticalCurrentAngle:
            return True
            
    
    #update current and goal angles
    def Update(self):
        self.motor.Set(self.verticalCurrentAngle)
        self.currentAngle = self.currentAngle.GetPosition()