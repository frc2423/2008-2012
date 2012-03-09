'''
    fake_wpilib    
    
    This is a library designed to emulate parts of WPILib so you can
    more easily do unit testing of your robot code. 
    
    Robot Code Assumptions:
    
        - robot code is in robot.py, and 'import robot' works. 
        - the 'run' function is the first thing that is called,
        and returns an object that derives from SimpleRobot
        - fake_wpilib is placed next to robot.py
    
    This is not a complete implementation of WPILib. Add more things
    as needed, and submit patches! :) 

    Usage in robot code:
    
        try:
            import wpilib
        except ImportError:
            import fake_wpilib as wpilib
            
        ...

        def run():
            robot = MyRobot()
            robot.StartCompetition()
            
            return robot
           
            
    Usage in test program:
    
        import fake_wpilib as wpilib
        
        ... 
        
        if __name__ == '__main__':
        
            robot = wpilib.initialize_robot()
            
            # initialize events
            robot.on_IsOperatorControl = some_function
            ... etc
'''

__all__ = ['fake_time','pid_controller']

import math

# move these into the global namespace
from .pid_controller import PIDSource, PIDOutput, PIDController
from .fake_time import Notifier, Timer, Wait, GetClock




def initialize_robot():
    '''
        Call this function first to import the robot code and
        start it up
    '''

    import robot
    return robot.run()


#################################################
#
# Core engine of robot code
#
#################################################    

class SimpleRobot(object):
    
    # assign functions to these in test program to be called
    # when something happens in the robot code. The return value
    # will be given to the robot code
    on_IsAutonomous = None
    on_IsOperatorControl = None
    
    enabled = False
    
    def IsAutonomous(self):
        if self.on_IsAutonomous is not None:
            return self.on_IsAutonomous()
        return False
    
    def IsEnabled(self):
        return self.enabled
        
    def IsDisabled(self):
        return not self.enabled
        
    def IsOperatorControl(self):
        if self.on_IsOperatorControl is not None:
            return self.on_IsOperatorControl()
        return False
    
    def StartCompetition(self):
        pass
        
    def GetWatchdog(self):
        return Watchdog()
    
#################################################
#
# WPILib Functionality
#
#################################################
    
    
class Accelerometer(object):

    def __init__(self, channel):
        self.value = 0
        
    def GetAcceleration(self):
        return self.value
        
    def SetSensitivity(self, sensitivity):
        pass
        
    def SetZero(self, zero):
        pass
    

class AnalogChannel(object):

    kAccumulatorModuleNumber = 1
    kAccumulatorNumChannels = 2
    kAccumulatorChannels = [1, 2]
    
    def __init__(self, channel):
        self.voltage = 0
        pass
        
        
    def GetValue(self):
        return self.value
        
    def GetAverageValue(self):
        return self.average_value
        
    def GetVoltage(self):
        return self.voltage
        
    def GetAverageVoltage(self):
        return self.average_voltage
        
    # TODO: Implement a sensible implementation for this



    
class CANJaguar(object):

    # ControlMode
    kPercentVbus = 1
    kCurrent = 2
    kSpeed = 3
    kPosition = 4
    kVoltage = 5
    
    # Limits
    kForwardLimit = 1
    kReverseLimit = 2
    
    # PositionReference
    kPosRef_QuadEncoder = 0
    kPosRef_Potentiometer = 1
    kPosRef_None = 0xFF
    
    # SpeedReference
    kSpeedRef_Encoder = 0
    kSpeedRef_InvEncoder = 2
    kSpeedRef_QuadEncoder = 3
    kSpeedRef_None = 0xFF
    
    # NeutralMode
    kNeutralMode_Brake = 1
    kNeutralMode_Coast = 2
    
    # LimitMode
    kLimitMode_SwitchInputsOnly = 0
    kLimitMode_SoftPositionLimits = 1
    
    def __init__(self, deviceNumber, controlMode=kPercentVbus):
        self.control_mode = controlMode
        self.forward_ok = True              # forward limit switch
        self.reverse_ok = True              # reverse limit switch
        self.position = 0
        self.speed = 0
        self.value = 0
    
    def ChangeControlMode(self, mode):
        self.control_mode = mode
    
    def ConfigEncoderCodesPerRev(self, codes):
        pass
        
    def ConfigFaultTime(self, faultTime):
        pass
        
    def ConfigMaxOutputVoltage(self, voltage):
        pass
    
    def ConfigNeutralMode(self, mode):
        pass
    
    def ConfigPotentiometerTurns(self, turns):
        pass
        
    def ConfigSoftPositiionLimits(self, forward, reverse):
        pass
        
    def DisableControl(self):
        pass
        
    def DisableSoftPositionLimits(self):
        pass
        
    def EnableControl(self, encoder_position=0):
        pass
        
    def Get(self):
        return self.value
        
    def GetControlMode(self):
        return self.control_mode
        
    def GetFirmwareVersion(self):
        return 0
        
    def GetForwardLimitOK(self):
        return self.forward_ok
        
    def GetOutputCurrent(self):
        return 0.0
        
    def GetOutputVoltage(self):
        return 0.0
        
    def GetP(self):
        return self.p
        
    def GetI(self):
        return self.i
        
    def GetD(self):
        return self.d
        
    def GetReverseLimitOK(self):
        return self.reverse_ok
        
    def GetPosition(self):
        if self.control_mode != kPosition:
            raise RuntimeError("Invalid control mode")
        return self.position
        
    def GetSpeedReference(self):
        return self.speed_reference
        
    def GetSpeed(self):
        if self.control_mode != kSpeed:
            raise RuntimeError("Invalid control mode")
        return self.speed
        
    def Set(self, value, syncGroup=0):
        self.value = value
        
    def SetPID(self, p, i, d):
        self.p = p
        self.i = i
        self.d = d
        
    def SetPositionReference(self, positionReference):
        self.position_reference = positionReference
        
    def SetSpeedReference(self, speedReference):
        self.speed_reference = speedReference
        
    def SetVoltageRampRate(self, rampRate):
        pass
        
    @staticmethod
    def UpdateSyncGroup(group):
        pass
    
class Compressor(object):
    
    def __init__(self, pressureSwitchChannel, compressorRelayChannel):
        self.enabled = False
        self.value = 0
        
    def Enabled(self):
        return self.enabled
    
    def Start(self):
        self.enabled = True
        
    def Stop(self):
        self.enabled = False
        
    def GetPressureSwitchValue(self):
        return self.value

    
class DigitalInput(object):

    def __init__(self, channel):
        self.value = False
        self.channel = channel
    
    def Get(self):
        return self.value
        
    def GetChannel(self):
        return self.channel
        
    def Set(self, value):
        if value:
            self.value = True
        else:
            self.value = False
     
     
class DigitalOutput(object):
    
    def __init__(self, channel):
        self.value = False
        self.channel = channel

    def Get(self):
        return self.value
       
    def Set(self, value):
        self.value = value

class DriverStation(object):
    
    instance = None
    
    @staticmethod
    def GetInstance():
        if DriverStation.instance is None:
            DriverStation.instance = DriverStation()
        return DriverStation.instance
    
    def __init__(self):
        self.digital_in = [ False, False, False, False, False, False, False, False ]
        self.fms_attached = False
        self.enhanced_io = DriverStationEnhancedIO()
    
    def GetDigitalIn(self, number):
        return self.digital_in[number-1]
        
    def GetEnhancedIO(self):
        return self.enhanced_io
    
    def IsFMSAttached(self):
        return self.fms_attached 
        
    def IsNewControlData(self):
        return True
    
    def SetDigitalOut(self, number, value):
        pass
        
class DriverStationEnhancedIO(object):
    
    kUnknown = 1
    kInputFloating = 2 
    kInputPullUp = 3
    kInputPullDown = 4
    kOutput = 5
    kPWM = 6
    kAnalogComparator = 7

    # don't call this directly
    def __init__(self):
        self.digital = [ False, False, False, False, 
                         False, False, False, False,
                         False, False, False, False,
                         False, False, False, False ]
                        
        self.digital_config = [ None, None, None, None,
                                None, None, None, None,
                                None, None, None, None,
                                None, None, None, None ]
        
    def SetDigitalConfig( self, channel, config ):
        self.digital_config[channel-1] = config
    
    def SetDigitalOutput( self, channel, value ):
        print( "DOUT: %s %s" % (channel, value) )
        self.digital[channel-1] = bool(value)
        
    
        
class Encoder(object):

    def __init__(self, port1, port2 ):
        self.value = None
        
    def SetPIDSourceParameter(self, thing):
        pass
        
    def kRate(self, rate):
        pass
    def SetDistancePerPulse(self, distance):
        pass
    def Start(self):
        self.value = 0
        
    def Get(self):
        return self.value

        
class Gyro(object):
    
    kSamplesPerSecond = 50.0
    kCalibrationSampleTime = 5.0
    kDefaultVoltsPerDegreePerSecond = 0.007
    
    def __init__(self, channel):
        self.value = 0
        pass
        
    def GetAngle(self):
        return self.value
    
    def Reset(self):
        self.value = 0
    
    def SetSensitivity(self, voltsPerDegreePerSecond):
        pass
        
        
class Jaguar(object):

    def __init__(self, channel):
        self.value = 0
        
    def Get(self):
        return self.value
        
    def Set(self, value):
        self.value = value
        
        
class Joystick(object):
    
    kTriggerButton = 0
    kTopButton = 1
    
    def __init__(self, port):
        self.x = 0.0
        self.y = 0.0
        self.z = 0.0
        self.twist = 0.0
        self.throttle = 0.0
        
        # trigger, top, 3... 
        self.buttons = [ False, False, False, False, False, False, False, False, False, False, False ]
        
    def GetButton(self, button_type):
        if button_type == Joystick.kTriggerButton:
            return self.GetTrigger()
        elif button_type == Joystick.kTopButton:
            return self.GetTop()
            
        raise RuntimeError( 'Invalid button type specified' )
    
    def GetDirectionDegrees(self):
        return (180.0/math.acos(-1.0)) * self.GetDirectionRadians()
    
    def GetDirectionRadians(self):
        return math.atan2( self.x, -self.y )
        
    def GetMagnitude(self):
        return math.sqrt( math.pow( self.x, 2) + math.pow( self.y, 2 ) )
        
    def GetRawButton(self, number):
        return self.buttons[number-1]
        
    def GetThrottle(self):
        return self.throttle
        
    def GetTop(self):
        return self.buttons[1]
        
    def GetTrigger(self):
        return self.buttons[0]
        
    def GetTwist(self):
        return self.twist
        
    def GetX(self):
        return self.x
        
    def GetY(self):
        return self.y
        
    def GetZ(self):
        return self.z
    
class KinectStick(Joystick):
    pass
    
class RobotDrive(object):

    kFrontLeftMotor = 0
    kFrontRightMotor = 1
    kRearLeftMotor = 2
    kRearRightMotor = 3 
    
    def __init__(self, lf_motor, rf_motor):
        self.x = 0.0
        self.y = 0.0
        
    def ArcadeDrive(self, y, x, tight=False):
        self.x = x
        self.y = y
        
    def SetSafetyEnabled(self, enabled):
        pass
        
    def SetInvertedMotor(self, motor, isInverted):
        pass


class Relay(object):
    
    # Value
    kOff = 0
    kOn = 1
    kForward = 2
    kReverse = 3
    
    # Direction
    kBothDirections = 0
    kForwardOnly = 1
    kReverseOnly = 2
    
    def __init__(self, channel, direction=kBothDirections):
        self.on = False
        self.forward = False
        self.value = Relay.kOff
        
    def Set(self, value):
        
        self.value = value
        
        if value == Relay.kOff or value == Relay.kReverse:
            self.forward = False
            self.on = False
            
        elif value == Relay.kOn or value == Relay.kForward:
            self.forward = True
            self.on = True
            
        else:
            raise RuntimeError( 'Invalid value %s passed to Relay.Set' % str(value) )
            
            
class Servo(object):

    kMaxServoAngle = 170.0
    kMinServoAngle = 0.0

    def __init__(self, channel):
        self.value = None
        
    def Get(self):
        return self.value
    
    def GetAngle(self):
        return self.value * self.__GetServoAngleRange() + Servo.kMinServoAngle
    
    def GetMaxAngle(self):
        return Servo.kMaxAngle
        
    def GetMinAngle(self):
        return Servo.kMinAngle
    
    def Set(self, value):
        self.value = value
        
    def SetAngle(self, degrees):
        if degrees < Servo.kMinServoAngle:
            degrees = Servo.kMinServoAngle
        elif degrees > Servo.kMaxServoAngle:
            degrees = Servo.kMaxServoAngle
            
        self.Set( (degrees - Servo.kMinServoAngle) / self.__GetServoAngleRange() )
    
    def __GetServoAngleRange(self):
        return Servo.kMaxServoAngle - Servo.kMinServoAngle


class Solenoid(object):
    
    def __init__(self, channel):
        self.value = False
        
    def Get(self):
        return self.value
        
    def Set(self, value):
        self.value = value

        
class Ultrasonic(object):

    @staticmethod
    def SetAutomaticMode( mode ):
        pass

    def __init__(self, pingChannel, echoChannel):
        self.distance = 0
        self.enabled = True
        self.range_valid = True
        
    def GetRangeInches(self):
        return self.distance
        
    def GetRangeMM(self):
        return self.GetRangeInches() * 25.4
        
    def IsEnabled(self):
        return self.enabled
        
    def IsRangeValid(self):
        return self.enabled and self.range_valid
        
    def Ping(self):
        pass
        
    def SetEnabled(self, enable):
        self.enabled = enable
    
    
class Victor(object):
    
    def __init__(self, channel):
        self.value = 0
        
    def Get(self):
        return self.value
        
    def Set(self, value):
        self.value = value
    
    
class Watchdog(object):
    
    def Feed(self):
        pass
        
    def SetEnabled(self, enable):
        pass
        
    def SetExpiration(self, period):
        pass
        
