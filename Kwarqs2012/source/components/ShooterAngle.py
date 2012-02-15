class VerticalAngle(object):
    
    
    
    def __init__(self,degrees,mNumber):
        self.verticalGoalAngle = 0
        self.verticalCurrentAngle = 0 
        self.motor = CANJaguar(mNumber)
    
    #set angle to angle given in goalAngle
    def SetGoal(self,degrees)
        self.goalVeticalAngle = degrees
      
    
    #check if GoalAngle = CurrentAngle
    def IsReady()
        if verticalGoalAngle == verticalCurrentAngle:
            return True
            
    
    #update current and goal angles
    def Update()
        self.motor.set