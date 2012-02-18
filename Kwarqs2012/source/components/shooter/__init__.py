from components.shooter.shooter_angle import VerticalAngle
from components.shooter.shooter_susan import Susan
from components.shooter.shooter_wheel import Wheel 

try:
    import basket_tracker 
except ImportError:
    import fake_basket_tracker as basket_tracker
try:
    import wpilib
except ImportError:
    import fake_wpilib as wpilib



class Shooter(object):
    
    
    def __init__( self, angleCAN, anglePot, susanCAN, susanGyro, wheelCAN1, wheelCAN2, wEncoder1, wEncoder2 ):

        self.vAngle = VerticalAngle(angleCAN, anglePot)
        self.susan = Susan(susanCAN, susanGyro)
        self.wheel = Wheel(wheelCAN1, wheelCAN2, wEncoder1, wEncoder2)
        #new
        self.cameraData = basket_tracker.BasketTracker()
    
    #cameraData.Get() - Not sure what exactly do do with this since I'm not familiar with how 
    #    the data returns
    
    #check if Angle is ready, Susan is ready, Wheel is read
    def IsReady(self):
        self.ready = False
        if self.angle.IsReady() and self.susan.IsReady() and self.wheel.IsReady():
            self.ready = True
        return True 
        
    def Update(self):
        self.angle.Update() and self.susan.Update() and self.wheel.Update()
        #SetGoal(      )