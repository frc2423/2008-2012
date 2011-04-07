
try:
    import wpilib
except:
    import fake_wpilib as wpilib

class PrintTimer(object):

    def __init__(self, timeout=1.0):
        self.timer = wpilib.Timer()
        self.timer.Start()
        self.timeout = timeout
        self.last_time = self.timer.Get()
        self.started = False
        
    def should_print(self):
        
        if self.last_time + self.timeout < self.timer.Get():
            self.last_time = self.timer.Get()
            return True
            
        return False
        

