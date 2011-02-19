
import wpilib

class SometimesPrint(object):

    def __init__(self, timeout=1.0):
        self.timer = wpilib.Timer()
        self.timer.Start()
        self.timeout = timeout
        
    def print(self, str):
        if self.timer.HasPeriodPassed(self.timeout):
            print(str)
            self.timer.Reset()

