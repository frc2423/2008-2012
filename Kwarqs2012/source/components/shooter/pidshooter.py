try:
    import wpilib
except ImportError:
    import fake_wpilib as wpilib
    
from util import PrintTimer

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
        self.wheelMotor1 = wheelMotor1
        self.wheelMotor2 = wheelMotor2
        self.print_timer = PrintTimer()
        self.desiredPVBus = 0
        
    def PIDWrite(self, output):
    
        # TODO: This is not correct
        self.desiredPVBus += output
        if self.desiredPVBus > 1:
            setPVBus = 1
        elif self.desiredPVBus < -1:
            setPVBus = -1
        else:
            setPVBus = self.desiredPVBus
        
        self.wheelMotor1.Set(setPVBus, ShooterWheelOutput.SYNCGROUP)
        self.wheelMotor2.Set(-setPVBus, ShooterWheelOutput.SYNCGROUP)
        wpilib.CANJaguar.UpdateSyncGroup(ShooterWheelOutput.SYNCGROUP)
        
        if self.print_timer.should_print():
            print("ShooterWheelOutput: %f -> %f" % (output, setPVBus)) 
