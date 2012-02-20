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
    def ToggleAutoSusan(self):
        self.autoSusan = not self.autoSusan
        self.susan.SetMode(self.autoSusan)
        
    def ToggleAutoAngle(self):
        self.autoAngle = not self.autoAngle
        self.vAngle.SetMode(self.autoAngle)
        
    def ToggleAutoWheel(self):
        self.autoWheel = not self.autoWheel
        self.wheel.SetMode(self.autoWheel)
        
    ''' Checks if component is in auto mode and if we are validaly shooting'''
    def AutoAndValid(self, component):
        valid = True
        if not component:
            valid = False
        if not self.susan.PointingCorrectly():
            valid = False
        if not self.cameraData.target_data_valid:
            valid = False
        return valid
    
    def Update(self):
        trackingData = self.cameraData.GetData()
        if self.AutoAndValid(self.autoSusan) :
            self.susan.SetGoalAngle(trackingData.angle_susan)
        
        if self.AutoAndValid(self.autoWheel):
            self.wheel.setDistance(trackingData.distance)
            
        if self.AutoAndValid(self.autoAngle):
            self.wheel.setDistance(trackingData.verticalAngle)
        
        self.vAngle.Update()
        self.Susan.Update()
        self.wheel.Update()
        
        
        #SetGoal(      )
        