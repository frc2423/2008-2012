from components.shooter.shooter_angle import VerticalAngle
from components.shooter.shooter_susan import Susan
from components.shooter.shooter_wheel import Wheel 
from components.shooter.pidsusan import SusanSource
from components.shooter.pidsusan import SusanOutputVoltage

try:
    import basket_tracker 
except ImportError:
    import fake_basket_tracker as basket_tracker
try:
    import wpilib
except ImportError:
    import fake_wpilib as wpilib



class Shooter(object):
    
    SUSAN_P = 1.0
    SUSAN_I = 0.0
    SUSAN_D = 0.0
    
    def __init__(self,vAngle,susan,wheel):
                 
        #self, angleCAN, anglePot, susanCAN, susanGyro, wheelCAN1, wheelCAN2):

        self.vAngle = vAngle
        self.susan = susan
        self.wheel = wheel
        #new
        self.cameraData = basket_tracker.BasketTracker()
        self.autoSusan = True
        self.autoAngle = True
        self.autoShooter = True
        
        self.susanSource = SusanSource()
        self.SusanOutputVoltage = SusanOutputVoltage()
        pidControl = wpilib.PIDController(Shooter.SUSAN_P, Shooter.SUSAN_I, Shooter.SUSAN_D, self.susanSource, self.SusanOutputVoltage)
    
    
    #cameraData.Get() - Not sure what exactly do do with this since I'm not familiar with how 
    #    the data returns
    
    #check if Angle is ready, Susan is ready, Wheel is read
    def IsReady(self):
        self.ready = False  
        if self.vAngle.IsReady() and self.susan.IsReady() and self.wheel.IsReady():
            self.ready = True
        return True 
    
    #toggles manual modes true/false  
    def toggleManualSusan(self):
        self.manualSusan = not self.manualSusan
        
    def toggleManualAngle(self):
        self.manualAngle = not self.manualAngle
        
    def toggleManualShooter(self):
        self.manualShooter = not self.manualShooter
        
    def Update(self):
        if self.autoSusan :
            self.susan.se
        self.angle.Update() and self.susan.Update() and self.wheel.Update()
        #SetGoal(      )
        