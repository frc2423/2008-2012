
class Compressor(object):
    def __init__(self,x,y):
        pass
        
    def Start(self):
        pass

class Solenoid(object):
    def __init__(self,x):
       self.is_set = False
        
    def Set(self,x):
        self.is_set = x
        
    def Get(self):
        return self.is_set
        
class Timer(object):
    def __init__(self):
        self.elapsed_time = 0.0
        
    def Start(self):
        self.elapsed_time = 0.0

    def Reset(self):
        self.elapsed_time = 0.0
        
    def Get(self):
        return self.elapsed_time 
        
    def Set(self,x): 
        self.elapsed_time = x
        
class Joystick(object):
    def __init__(self):
        pass
        
    def GetTrigger(self):
        return True  
        