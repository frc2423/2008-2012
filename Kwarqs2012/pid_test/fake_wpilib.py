'''
    fake_wpilib.py
    
    This is a library designed to emulate parts of WPILib so you can
    more easily do
    
    Robot Code Assumptions:
    
        - robot code is in robot.py
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



# This variable is incremented by the Wait function
global_time = 0


def initialize_robot():
    '''
        Call this function first to import the robot code and
        start it up
    '''

    import robot
    return robot.run()


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
    
class CANJaguar(object):

    # control mode
    kPercentVbus = 1
    kCurrent = 2
    kSpeed = 3
    kPosition = 4
    kVoltage = 5
    
    # position mode
    kPosRef_QuadEncoder = 0
    kPosRef_Potentiometer = 1
    
    # neutral mode
    kNeutralMode_Brake = 1
    kNeutralMode_Coast = 2
    
    def __init__(self, deviceNumber):
        self.control_mode = CANJaguar.kPercentVbus
        self.forward_ok = True
        self.reverse_ok = True
        self.position = 0
        self.value = 0
        
    def ConfigEncoderCodesPerRev(self, codes):
        pass
        
    def ConfigPotentiometerTurns(self, turns):
        pass
        
    def ChangeControlMode(self, mode):
        self.control_mode = mode
        
    def ConfigNeutralMode(self, mode):
        pass
        
    def DisableControl(self):
        pass
        
    def EnableControl(self, encoder_position=0):
        pass
        
    def Get(self):
        return self.value
        
    def GetControlMode(self):
        return self.control_mode
        
    def GetForwardLimitOK(self):
        return self.forward_ok
        
    def GetOutputCurrent(self):
        return 0.0
        
    def GetOutputVoltage(self):
        return 0.0
        
    def GetReverseLimitOK(self):
        return self.reverse_ok
        
    def GetPosition(self):            
        return self.position
        
    def Set(self, value):
        self.value = value
        
    def SetPID(self, p, i, d):
        pass
        
    def SetPositionReference(self, ref):
        pass
        
    
    
class DigitalInput(object):

    def __init__(self, port):
        self.value = False
        
    def Get(self):
        return self.value
        
    def Set(self, value):
        if value:
            self.value = True
        else:
            self.value = False

class DriverStation(object):
    
    @staticmethod
    def GetInstance():
        return DriverStation()
    
    def __init__(self):
        self.digital_in = [ False, False, False, False, False, False, False, False ]
        self.fms_attached = False
    
    def GetDigitalIn(self, number):
        return self.digital_in[number-1]
    
    def IsFMSAttached(self):
        return self.fms_attached 
    
    def SetDigitalOut(self, number, value):
        pass
        
        
class Encoder(object):

    def __init__(self, port1, port2 ):
        self.value = None
        
    def Start(self):
        self.value = 0
        
    def Get(self):
        return self.value

        
class Jaguar(object):

    def __init__(self, port):
        self.value = 0
        
    def Set(self, value):
        self.value = value
        
    def Get(self):
        return self.value
        
        
class Joystick(object):
    
    kTriggerButton = 0
    kTopButton = 1
    
    def __init__(self, port):
        self.x = 0
        self.y = 0
        self.z = 0
        
        # trigger, top, 3... 
        self.buttons = [ False, False, False, False, False, False, False, False, False, False, False ]
        
    def GetButton(self, button_type):
        if button_type == Joystick.kTriggerButton:
            return self.GetTrigger()
        elif button_type == Joystick.kTopButton:
            return self.GetTop()
            
        raise RuntimeError( 'Invalid button type specified' )
        
    def GetRawButton(self, number):
        return self.buttons[number-1]
        
    def GetTop(self):
        return self.buttons[1]
        
    def GetTrigger(self):
        return self.buttons[0]
        
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
    
    def __init__(self, l_motor, r_motor):
        self.x = 0.0
        self.y = 0.0
        
    def ArcadeDrive(self, y, x, tight):
        self.x = x
        self.y = y
        
    def SetSafetyEnabled(self, enabled):
        pass
        
    def SetInvertedMotor(self, motor, isInverted):
        pass
        
        
        
        
    
class Relay(object):
    
    kOff = 0
    kOn = 1
    kForward = 2
    kReverse = 3
    
    def __init__(self, port):
        self.value = Relay.kOff
        
    def Set(self, value):
        self.value = value
        
        
class Timer(object):
    
    def __init__(self):
        self.start_time = 0
        self.accumulated_time = 0
        self.running = False
        self.Reset()
        
    def Get(self):
        global global_time
        if self.running:
            return (global_time - self.start_time) + self.accumulated_time
        else:
            return self.accumulated_time
        
    def Reset(self):
        global global_time
        self.accumulated_time = 0
        self.start_time = global_time
        
    def Start(self):
        global global_time
        if not self.running:
            self.start_time = global_time
            self.running = True
        
    def Stop(self):
        if self.running:
            self.accumulated_time += self.Get()
            self.running = False
        
    def HasPeriodPassed(self, period):
        global global_time
        if self.Get() > period:
            self.start_time += period
            return True
        return False
        
class Ultrasonic(object):

    @staticmethod
    def SetAutomaticMode( mode ):
        pass

    def __init__(self, pingChannel, echoChannel):
        self.distance = 240
    
    def GetRangeInches(self):
        return self.distance
        
    def IsRangeValid(self):
        return True
    
        
def Wait(time):
    global global_time
    global_time += time

class Watchdog(object):
    
    def Feed(self):
        pass
        
    def SetEnabled(self, enable):
        pass
        
    def SetExpiration(self, period):
        pass
        
