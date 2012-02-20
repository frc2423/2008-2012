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
    

    
    def __init__(self,vAngle,susan,wheel):
                 
        #self, angleCAN, anglePot, susanCAN, susanGyro, wheelCAN1, wheelCAN2):

        self.vAngle = vAngle
        self.susan = susan
        self.wheel = wheel
        #new
        self.cameraData = basket_tracker.BasketTracker()
        self.autoSusan = True
        self.autoAngle = True
        self.autoWheel = True
        

        
    #cameraData.Get() - Not sure what exactly do do with this since I'm not familiar with how 
    #    the data returns
    
    #check if Angle is ready, Susan is ready, Wheel is read
    def IsReady(self):
        self.ready = False  
        if self.vAngle.IsReady() and self.susan.IsReady() and self.wheel.IsReady():
            self.ready = True
        return True 
    
    #toggles manual modes true/false  
    def ToggleManualSusan(self):
        self.autoSusan = not self.manualSusan
        
    def ToggleManualAngle(self):
        self.autoAngle = not self.manualAngle
        
    def ToggleManualShooter(self):
        self.autoWheel = not self.manualShooter
        
    def AutoAndValidSusan(self):
        valid = True
        if not self.autoSusan:
            valid = False
        if not self.susan.PointingCorrectly():
            valid = False
        if not self.cameraData.target_data_valid:
            valid = False
        return valid
    def Update(self):
        trackingData = self.cameraData.GetData()
        if Shooter.AutoAndValidSusan() :
            self.susan.SetGoalAngle(trackingData.angle_susan)
        
        if self.autoWheel():
            self.wheel.setDistance(trackingData.distance)
            
        
        
        self.vAngle.Update()
        self.Susan.Update()
        self.wheel.Update()
        
        
        #SetGoal(      )
        