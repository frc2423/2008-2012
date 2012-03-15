

# print something out so we know what code is running
print( "Loading: Kwarqs2012 wheel testing program" )

try:
    import wpilib
    import wpilib.SmartDashboard
except ImportError:
    import fake_wpilib as wpilib
    import fake_wpilib.SmartDashboard

import math
import threading


class PidEncoder(wpilib.PIDSource):
    def __init__(self, encoder):
        wpilib.PIDSource.__init__(self)
        self._lock = threading.RLock()
        self.encoder = encoder
        self.value = 0.0
        
    def Get(self):
        with self._lock:
            return self.value
        
    def PIDGet(self):
        with self._lock:
            previous_encoder_val = 
            actual_encoder_val   = self.encoder.PIDGet()
            self.value           = alpha * actual_encoder_val + (1-alpha) * previous_encoder_val
            return self.value


class PidMotor(wpilib.PIDOutput):
    def __init__(self, jaguar):
        wpilib.PIDOutput.__init__(self)
        self._lock = threading.RLock()
        self.jaguar = jaguar
        self.value = 0.0
        
    def Get(self):
        with self._lock:
            return self.value
        
    def PIDWrite(self, value):
        #with self._lock:
        self.value += value
        self.jaguar.Set( self.value )


stick1 = wpilib.Joystick(1)

# geting the motor object from where we get the signa given to the motor
motor = wpilib.Jaguar(2)
# geting the encoder object from where we get the encoder value at each moment
encoder = wpilib.Encoder(10,11, wpilib.Encoder.k1X)
    
# signal going to the motor using PIDWrite
p_motor = PidMotor( motor )
# encoder values = motor speed
p_encoder = PidEncoder( encoder )


#P         = 0.000675
#I         = 0.00000005 
#D         = 0.01
P         = 0.1
I         = 0.0001
D         = 0.1
#setpoint  = 0.1
#tolerance = 0.05

pid = wpilib.PIDController(P,I,D, p_encoder, p_motor)
#pid.SetSetpoint(setpoint)
#pid.SetTolerance(tolerance)

smart_pid = wpilib.SmartDashboard.SendablePIDController( P, I, D, p_encoder, p_motor )
sd = wpilib.SmartDashboard.SmartDashboard.GetInstance()
sd.PutData( "PID1", smart_pid )


class MyRobot(wpilib.SimpleRobot):

    def __init__(self):
        '''Constructor'''
        
        wpilib.SimpleRobot.__init__(self)
        encoder.SetPIDSourceParameter( wpilib.Encoder.kRate )
        print("encoder krate: %s" % (wpilib.Encoder.kRate))
        #encoder.SetPIDSourceParameter(10)
        # WHAT IS THIS??
        encoder.SetDistancePerPulse( 1/3600.0 )      # this is for the small wheel we have
        encoder.Start()
        
        self.errors = []
        
            
    def RobotInit(self):
        '''Called only once during robot initialization'''
        pass

    def Disabled(self):
        '''Called when the robot is disabled'''
        
        print("MyRobot::Disabled()")
    
        while self.IsDisabled():
            wpilib.Wait(0.01)

    def Autonomous(self):
        '''Called during autonomous mode'''
        
        print("MyRobot::Autonomous()")
    
        self.GetWatchdog().SetEnabled(False)
        while self.IsAutonomous() and self.IsEnabled():
            wpilib.Wait(0.01)
            
    
    def Print(self):
    
        prate = p_encoder.Get()
        pmotor = p_motor.Get()
        setpoint = pid.GetSetpoint()
    
        diff = None
        avg_diff = None
        
        if prate > setpoint:
            diff = prate - setpoint
        else:
            diff = setpoint - prate
        
        self.errors.insert(0, diff)
        
        if len(self.errors) > 2:
            self.errors.pop()
            
        avg_diff = sum( self.errors ) / len(self.errors)
        
        #print( "    WR: %s; GR: %s; D: %s; M: %s" % (rate, setpoint, avg_diff, motor))
        #print("sp: %s ; diff: %s; rate: %s motor: %s \n " % (setpoint, diff, rate, motor))
        print("P: %3f, I: %3f, D: %3f, S: %3f, ER: %3f, T: %s, PM: %3f, M: %3f, PR: %3f, R: %3f, E: %s\n" %(
            pid.GetP(), pid.GetI(), pid.GetD(), 
            pid.GetSetpoint(), pid.GetError(), pid.OnTarget(), 
            pmotor, motor.Get(), prate, encoder.GetRate(), encoder.Get()
        ))
        sd.PutDouble( 'encoder.GetRate()', encoder.GetRate() )
        sd.PutDouble( 'p_encoder.Get()', prate )
        sd.PutDouble( 'Set point', setpoint )
        #sd.PutDouble( 'average difference', avg_diff )
        #sd.PutDouble( 'signal going to motor', motor )
    
            
    def OperatorControl(self):
        '''Called during Teleoperated Mode'''
    
        print("MyRobot::OperatorControl()")
    
        dog = self.GetWatchdog()
        dog.SetEnabled(True)
        dog.SetExpiration(0.25)
        
        timer = wpilib.Timer()
        timer.Start()
        
        enabled = False

        # main loop
        while self.IsOperatorControl() and self.IsEnabled():
   
            dog.Feed()
            
            if stick1.GetTrigger():
            
                if not enabled:
                    # reset motor value every time we reset the PID controller
                    p_motor.value = motor.Get()
                    pid.Enable()
                    enabled = True
                #scaling v from 0 to 1, because stick.GetZ is from -1 to 1
                v = (stick1.GetZ() + 1.0) / 2.0
                pid.SetSetpoint( v * 7.5 )
                #pid.SetSetpoint( 20 )
                
            else:
                if enabled:
                    pid.Disable()
                    enabled = False
                    motor.Set(0.0)
                    
                if stick1.GetTop():
                    motor.Set( 1.0 )
                else:
                    motor.Set( 0.0 )
            
            
            if timer.HasPeriodPassed( .1 ):
                self.Print()
            
            wpilib.Wait(0.04)

        
def run():
    '''This function must be present for the robot to start'''
    robot = MyRobot()
    robot.StartCompetition()
    
    # must return a value to work with the tester program
    return robot
