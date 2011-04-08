
try:
    import wpilib
except:
    import fake_wpilib as wpilib

class PrintTimer(object):

    def __init__(self, timeout=1.0):
        self.timer = wpilib.Timer()
        self.timer.Start()
        self.timeout = timeout
        # we subtract the timeout because we want should_print()
        # to return true the very first time that this is called
        self.last_time = self.timer.Get() - (timeout*2)
        self.ids = []
        
    def Get(self):
        return self.timer.Get()
        
    def should_print(self, id=None):
        '''
        This returns true if the timeout interval has expired. 
        If you specify an ID, then this will return true for
        every ID that hasn't printed in the last interval
        '''
        
        if self.last_time + self.timeout < self.timer.Get():
            self.last_time = self.timer.Get()
            self.ids = [id]
            return True
        elif id not in self.ids:
            self.ids.append( id )
            return True
            
        return False
        
