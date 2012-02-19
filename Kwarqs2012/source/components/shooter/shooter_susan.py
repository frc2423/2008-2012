try:
    import wpilib
except ImportError:
    import fake_wpilib as wpilib



class Susan(object):
    

    
    def __init__(self,susanCAN,susanGyro):
        self.susanMotor = wpilib.CANJaguar(susanCAN)
        self.gyro = wpilib.Gyro(susanGyro)
        self.goalSpeed = 0
    '''override function when called turn'''
    def turn(self,mNumber):
        self.susanMotor.set(Susan.forwards)    
        
    '''set goal speed variable'''
    def SetGoalSpeed(self,speed):
        self.goalSpeed = speed
    
    '''updates variables'''
    def Update(self):
        self.susanMotor.set(Susan.forwards)