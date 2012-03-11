
from .pid_controller import PIDController

class SmartDashboard(object):

    instance = None
    
    @staticmethod
    def GetInstance():
        if SmartDashboard.instance is None:
            SmartDashboard.instance = SmartDashboard()
        return SmartDashboard.instance
        
    def __init__(self):
        self.data = {}
        
    def GetBoolean(self, key):
        if key in self.data:
            return self.data[key]
        return None
        
    def GetData(self, key):
        if key in self.data:
            return self.data[key]
        return None
        
    def GetInt(self, key):
        if key in self.data:
            return self.data[key]
        return None
        
    def PutBoolean(self, key, value):
        self.data[key] = bool(value)
        
    def PutData(self, key, value):
        self.data[key] = value
        
    def PutInt(self, key, value):
        self.data[key] = int(value)
        
    
    
    
class SendablePIDController(PIDController):
    """A {@link SendablePIDController} is a {@link PIDController} that can be
    sent over to the {@link SmartDashboard} using the
    {@link SmartDashboard#PutData()} method.

    It is useful if you want to test values of a {@link PIDController} without
    having to recompile code between tests.
    Also, consider using {@link Preferences} to save the important values
    between matches.

    @see SmartDashboard"""
    kP = "p"
    kI = "i"
    kD = "d"
    kSetpoint = "setpoint"
    kEnabled = "enabled"

    def __init__(self, p, i, d, source, output, period=0.05):
        """Allocate a PID object with the given constants for P, I, D.
        @param p the proportional coefficient
        @param i the integral coefficient
        @param d the derivative coefficient
        @param source The PIDSource object that is used to get values
        @param output The PIDOutput object that is set to the output value
        @param period the loop time for doing calculations in seconds. This
        particularly effects calculations of the integral and differential
        terms.  The default is 50ms."""
        super().__init__(p, i, d, source, output, period)
        self.table = None

    def SetSetpoint(self, setpoint):
        """Set the setpoint for the PIDController
        @param setpoint the desired setpoint"""
        super().SetSetpoint(setpoint)

        if self.table is not None:
            self.table[self.kSetpoint] = float(setpoint)

    def SetPID(self, p, i, d):
        """Set the PID Controller gain parameters.
        Set the proportional, integral, and differential coefficients.
        @param p Proportional coefficient
        @param i Integral coefficient
        @param d Differential coefficient"""
        super().SetPID(p, i, d)

        if self.table is not None:
            self.table[self.kP] = float(p)
            self.table[self.kI] = float(i)
            self.table[self.kD] = float(d)

    def Enable(self):
        """Begin running the PIDController"""
        super().Enable()

        if self.table is not None:
            self.table[self.kEnabled] = True

    def Disable(self):
        """Stop running the PIDController, this sets the output to zero before
        stopping."""
        super().Disable()

        if self.table is not None:
            self.table[self.kEnabled] = False