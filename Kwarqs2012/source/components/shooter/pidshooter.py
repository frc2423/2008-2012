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
    
''' Velocity control for shooter wheels ''' 
class ShooterWheelOutput(wpilib.PIDOutput):
    
    SYNCGROUP = 2
    
    def __init__(self, wheelMotor1, wheelMotor2):
        wpilib.PIDOutput.__init__(self)
        self.outputVoltage = 0.0
        self.wheelMotor1 = wheelMotor1
        self.wheelMotor2 = wheelMotor2
        self.desiredPVBus = 0
        
    def PIDWrite(self, output):
        self.desiredPVBus =  self.wheelMotor1.Get() + output
        if self.desiredPVBus > 1:
            setPVBus = 1
        elif self.desiredPVBus < 1:
            setPVBus = -1
        else:
            setPVBus = self.desiredPVBus
        
        self.wheelMotor1.Set(setPVBus, ShooterWheelOutput.SYNCGROUP)
        self.wheelMotor2.Set(setPVBus, ShooterWheelOutput.SYNCGROUP)
           
    def UpdateMotors(self):
        wpilib.UpdateSyncGroup(ShooterWheelOutput.SYNCGROUP)
        
    def GetPIDOutput(self):
        return self.outputVoltage