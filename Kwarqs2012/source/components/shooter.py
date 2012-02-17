try:
    import basket_tracker
except ImportError:
    import fake_basket_tracker as basket_tracker



class Shooter(object):
    
    
    def __init__(self, AngleCAN, SusanCAN, WheelCAN) :

        angle = Angle(AngleCAN)
        susan = Susan(SusanCAN)
        wheel = Wheel(WheelCAN)
        
        #new
        cameraData = BasketTracker()
    
    #cameraData.Get() - Not sure what exactly do do with this since I'm not familiar with how 
    #    the data returns
    
    #check if Angle is ready, Susan is ready, Wheel is read
    def IsReady(self):
        self.ready = false
        if angle.IsReady() and susan.IsReady() and wheel.IsReady():
            self.ready = True
        return ready 
    def Update(self):
        angle.Update() and susan.Update() and wheel.Update()
        #SetGoal(      )