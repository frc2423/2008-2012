self.Feeder.motorstate = 0
self.Feeder.Full = 0
self.Feeder.direction = kOff

class Feeder(object, relayChannel): 
    
    def __init__(self,feedmotor1):
        self.feeder = Relay(feedmotor1)
        
    def FeedOveride(self):
        self.direction = kForward
      
    '''  
    def Stop(self):
    '''    
    
    def Feed(self):
        self.direction = kForward
    
    def Expel(self):
        self.direction = kReverse
    
    def Update(self):
        self.feeder.set(self.direction) 
        
        