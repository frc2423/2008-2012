
try:
    import wpilib
    import wpilib.SmartDashboard
except ImportError:
    import fake_wpilib as wpilib
    import fake_wpilib.SmartDashboard
    
from util import PrintTimer
    
import threading

    
class WheelPidSource(wpilib.PIDSource):
    '''Encoder input for wheel PID'''

    def __init__(self, encoder):
        wpilib.PIDSource.__init__(self)
        self._lock = threading.RLock()
        self.encoder = encoder
        self.value = 0.0
        self.vals = []
        self.previous = 0.0
        
    def Get(self):
        with self._lock:
            return self.value
            
    def Reset(self):
        with self._lock:
            self.vals = []
            self.value = None
        
    def PIDGet(self):
        
        with self._lock:
            
            self.value = self.encoder.PIDGet()
        
            #alpha = 0.98
            #self.value = value * alpha + self.previous * (1-alpha)
            #self.previous = value
        
            #self.vals.insert(0, self.encoder.PIDGet())
            #if len(self.vals) > 1:
            #    self.vals.pop()
            #self.value = sum( self.vals ) / len(self.vals)
            return self.value

 
class WheelPidOutput(wpilib.PIDOutput):
    '''Jaguar output for wheel PID'''
    
    def __init__(self, wheelMotor1, wheelMotor2):
        wpilib.PIDOutput.__init__(self)
        
        self._lock = threading.RLock()
        self.wheelMotor1 = wheelMotor1
        self.wheelMotor2 = wheelMotor2
        
        self.value = 0
    
    def Get(self):
        with self._lock:
            return self.value
    
    def Reset(self):
        '''Always call this before enabling the PID controller'''
        value = self.wheelMotor1.Get()
    
        with self._lock:
            self.value = value
    
    def PIDWrite(self, output):
    
        with self._lock:
            self.value += output
            value = self.value
    
        self.wheelMotor1.Set(value, Wheel.SYNCGROUP)
        self.wheelMotor2.Set(-value, Wheel.SYNCGROUP)
        wpilib.CANJaguar.UpdateSyncGroup(Wheel.SYNCGROUP)

        
         
class Wheel(object):
    '''
        The robot has a wheel on top of it used to shoot balls. We have an
        encoder attached to it that allows us to (ideally) precisely control
        the speed of the motor. 
        
        If we can get the PID stuff to work really well, there really isn't
        any reason the user should ever use Vbus mode
    '''
    
    # used to determine the rate
    ENCODER_SINGLE_ROTATION = 16
    
    # PID constants
    MOTOR_P = 0.000675
    MOTOR_I = 0.000000050
    MOTOR_D = 0.01
    
    #MOTOR_P = 0.0
    #MOTOR_I = 0.0001
    #MOTOR_D = 0.0
    
    # sync group
    SYNCGROUP = 2
    
    # percent tolerance to report convergence
    PID_TOLERANCE = 15.0
    
    # minimum rate for the encoder for the wheel to declare itself 'ready'
    MIN_ENCODER_READY_RATE = 5.0
    
    # PID Wheel: minimum time the wheel should be at the right speed
    # before reporting that it is ready
    STABLE_READY_PERIOD = 0.5
    
    def __init__(self, wheelCAN1, wheelCAN2, encoder_tuple, rate_leds):
    
        self.vBus = None
        self.auto_speed = None
        
        # this starts as None so that when update gets called we
        # can immediately set the current mode regardless of the input
        self.autoMode = None
        
        # timer used to determine wheel speed stability
        self.on_target = False
        self.is_stable_timer = wpilib.Timer()
        self.is_stable_timer.Start()
        
        self.print_timer = PrintTimer()
        
        # shows the wheel rate in an LED
        self.rate_leds = rate_leds
        
        self.wheelMotor1 = wpilib.CANJaguar(wheelCAN1)
        self.wheelMotor2 = wpilib.CANJaguar(wheelCAN2)
        
        self.wheelMotor1.ConfigNeutralMode( wpilib.CANJaguar.kNeutralMode_Coast )
        self.wheelMotor2.ConfigNeutralMode( wpilib.CANJaguar.kNeutralMode_Coast )
        
        self.encoder = wpilib.Encoder(encoder_tuple[0],encoder_tuple[1])
        self.encoder.SetPIDSourceParameter(wpilib.Encoder.kRate)
        self.encoder.SetDistancePerPulse(1.0 / Wheel.ENCODER_SINGLE_ROTATION)
        self.encoder.Start()
        
        # setup PID stuff
        
        self.pid_source = WheelPidSource(self.encoder)
        self.pid_output = WheelPidOutput(self.wheelMotor1, self.wheelMotor2)
        
        self.pid_controller = wpilib.SmartDashboard.SendablePIDController(Wheel.MOTOR_P, Wheel.MOTOR_I, Wheel.MOTOR_D, self.pid_source, self.pid_output)
        self.pid_controller.SetInputRange( 0.0, 120.0 )
        self.pid_controller.SetTolerance( Wheel.PID_TOLERANCE )
        
    
        # warning: dont enable the PID controller here

        # SmartDashboard stuff
        
        self.sd = wpilib.SmartDashboard.SmartDashboard.GetInstance()
        self.sd.PutData( "Wheel PID", self.pid_controller )
        self.sd.PutDouble( "Wheel speed", 0 )
        self.sd.PutBoolean( "Speed OK", False )
        
        
    def _calculate_is_ready(self):
        '''Internal function'''
        if self.pid_controller.OnTarget():
            if not self.on_target:
                self.is_stable_timer.Reset()
                self.on_target = True
                
            s_tm = self.is_stable_timer.Get()
            if s_tm > Wheel.STABLE_READY_PERIOD:
                if self.print_timer.should_print( 'is_ready' ):
                    print( "[Wheel PID] Wheel speed is stable (speed: %.3f)" % self.auto_speed ) 
            
        else:
            self.on_target = False
    
    def IsReady(self):
        '''Returns True if the shooter is ready for a ball to be fed into it'''
        
        # safety feature to make sure the ball doesn't get stuck
        if self.encoder.GetRate() < Wheel.MIN_ENCODER_READY_RATE:
            return False
        
        # if we're controlling it by voltage, then it's automatically ready
        if self.autoMode == False:
            return True
        
        if not self.on_target:
            return False
        
        return self.is_stable_timer.Get() > Wheel.STABLE_READY_PERIOD
        
        
    def IsSet(self):
        '''Returns True if someone has already set the wheel speed'''
        return self.auto_speed is not None or self.vBus is not None
        
    
    def SetAutoSpeed(self, speed):
        '''This is a value between 0.0 and 99.0 indicating the desired speed
           of the wheel. This speed will be attained using PID control'''
        self.auto_speed = speed
    
            
    def SetVBus(self, vBus):
        ''' Set motor vbus directly (-1 to 1) '''
        self.vBus = vBus
        
        
    def Print(self):
        print( "ShooterWheel:")
        print( "    Wheel RPM: %f; Set Speed: %s; Auto Mode: %s" % (self.encoder.GetRate(), self.auto_speed, self.autoMode))
        
    
    def Update(self):
        '''Actually do stuff here'''
    
        enable_automode = False
    
        # if you set vbus, you win
        if self.vBus is not None:
            autoMode = False
        if self.auto_speed is not None:
            autoMode = True
        else:
            autoMode = False
            
        # detect transitions to/from automode
        if autoMode != self.autoMode:
            if not autoMode:
                self.pid_controller.Disable()
            else:
                enable_automode = True
                self.on_target = False
                
            self.autoMode = autoMode
    
        if self.autoMode:
            self.pid_controller.SetSetpoint(self.auto_speed)
            self._calculate_is_ready()
        else:
            if self.vBus is None:
                self.vBus = 0
            
            self.wheelMotor1.Set(self.vBus, Wheel.SYNCGROUP)
            self.wheelMotor2.Set(-self.vBus, Wheel.SYNCGROUP)
            wpilib.CANJaguar.UpdateSyncGroup(Wheel.SYNCGROUP)
            
        # do this last otherwise the PID control might be enabled with a nonsense setpoint
        if enable_automode:
            self.pid_source.Reset()
            self.pid_output.Reset()
            self.pid_controller.Enable()
            
        # tell the operator how fast the wheel is going via LEDs and SmartDashboard
        rate = self.encoder.GetRate()
        self.rate_leds.Set( rate )
        self.sd.PutDouble( "Wheel Speed", rate )
        self.sd.PutBoolean( "Speed OK", self.pid_controller.OnTarget() )
        
        # reset vars at the end
        self.auto_speed = None
        self.vBus = None
        
