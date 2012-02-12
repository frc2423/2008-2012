class Shooter(Object):
    def __init__(self, AngleCAN, SusanCAN, WheelCAN) :

    angle = Angle(AngleCAN)
    susan = Susan(SusanCAN)
    wheel = Wheel(WheelCAN)

    
    
    #check if Angle is ready, Susan is ready, Wheel is read
    def IsReady(self):
        self.ready = false
        if angle.IsReady() and susan.IsReady() and wheel.IsReady():
            self.ready = True
        return ready 
    def Update():
        angle.Update() and susan.Update() and wheel.Update()