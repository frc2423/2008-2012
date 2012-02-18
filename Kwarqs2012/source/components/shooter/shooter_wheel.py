
try:
    import wpilib
except ImportError:
    import fake_wpilib as wpilib


class Wheel(object):
    
    
    def __init__(self,mNumber):
        self.currentSpeed = 0
        self.goalSpeed = 0
        self.wheel = wpilib.CANJaguar(mNumber)
               
        
    '''   
        Description: sets variable that tells update to set the speed

        Variables:    
            float speed (-1 to 1) - speed to set the motor
    '''
    def SetSpeed(self,goalSpeed):
        self.goalSpeed = goalSpeed
        
        
    #checks if current speed = goal speed
    def IsReady(self):
        if self.currentSpeed == self.goalSpeed:
            return True
    
    
    #update the current speed and goal speed
    def Update(self):
        self.wheel.Set(self.goalSpeed)
        currentSpeed = self.wheel.GetSpeed()
        
            
            
    '''
        RM gives # of goalSpeed,mNumber
        mNumber never changes within an instance
        goalSpeed gets updated in update
    '''