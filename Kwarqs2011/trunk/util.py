
import wpilib

class PrintTimer(object):

    def __init__(self, timeout=1.0):
        self.timer = wpilib.Timer()
        self.timeout = timeout
        self.started = False
        
    def should_print(self):
        if not self.started:
            self.timer.Start()
            self.started = True
        return self.timer.HasPeriodPassed(self.timeout)

