try:
    import wpilib
except ImportError:
    import fake_wpilib as wpilib

#source for the PID controller
class SusanSource(wpilib.PIDSource):
    
    def __init__(self):
        wpilib.PIDSource.__init__(self)
        self.val = 0
    
    def setValue(self, val):
        self.val = val
    
    def PIDGet(self):
        return self.val
    
#output voltage to motor controled by PWM
class SusanOutputVoltage(wpilib.PIDOutput):
    
    def __init__(self):
        wpilib.PIDOutput.__init__(self)
        self.outputVoltage = 0.0
    
    def PIDWrite(self, output):
        self.outputVoltage = output
    
    def GetPIDOutput(self):
        return self.outputVoltage