
import wpilib

class PrintTimer(object):

    def __init__(self, timeout=1.0):
        self.timer = wpilib.Timer()
        self.timer.Start()
        self.timeout = timeout
        
    def should_print(self):
        return self.timer.HasPeriodPassed(self.timeout)

