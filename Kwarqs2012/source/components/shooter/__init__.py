from components.shooter.shooter_angle import VerticalAngle
from components.shooter.shooter_susan import Susan
from components.shooter.shooter_wheel import Wheel 


#try:
#    import basket_tracker 
#except ImportError:
#    import fake_basket_tracker as basket_tracker
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
        
        #self.basket_tracker = basket_tracker.BasketTracker()
        self.autoSusan = False
        self.autoAngle = False
        self.autoWheel = False
        

        
    #cameraData.Get() - Not sure what exactly do do with this since I'm not familiar with how 
    #    the data returns
    
    
    def IsShooting(self):
        # TODO
        return False
    
    #check if Angle is ready, Susan is ready, Wheel is read
    def IsReady(self):
        self.ready = False  
        if self.vAngle.IsReady() and self.susan.IsReady() and self.wheel.IsReady():
            self.ready = True
        return True 

            
    ''' Checks if component is in auto mode and if we are validaly shooting'''
    def AutoAndValid(self, component, trackingData):
        valid = True
        if not component:
            valid = False
        if not self.susan.PointingCorrectly():
            valid = False
        if not trackingData.target_data_valid:
            valid = False
        return valid
    
    def Update(self):
        # trackingData = self.basket_tracker.GetData()
        # if self.AutoAndValid(self.autoSusan, trackingData) :
            # self.susan.SetGoalAngle(trackingData.angle_susan)
        
        # if self.AutoAndValid(self.autoWheel, trackingData):
            # self.wheel.setDistance(trackingData.distance)
            
        # if self.AutoAndValid(self.autoAngle, trackingData):
            # self.vAngle.SetGoal(trackingData.verticalAngle)
        
        self.vAngle.Update()
        self.susan.Update()
        self.wheel.Update()
        
        
        #SetGoal(      )
        