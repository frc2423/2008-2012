

# print something out so we know what code is running
print( "Loading: Kwarqs2012 wheel testing program" )


import wpilib
import wpilib.SmartDashboard


class PidEncoder(wpilib.PIDSource):
    def __init__(self, encoder):
        wpilib.PIDSource.__init__(self)
        self.encoder = encoder
        
    def PIDGet(self):
        return self.encoder.PIDGet()


class PidMotor(wpilib.PIDOutput):
    def __init__(self, jaguar):
        wpilib.PIDOutput.__init__(self)
        self.jaguar = jaguar
        self.value = 0.0
        
    def PIDWrite(self, value):
        self.value += value
        self.jaguar.Set( self.value )


stick1 = wpilib.Joystick(1)

motor = wpilib.Jaguar(2)
encoder = wpilib.Encoder(10,11)
        
p_motor = PidMotor( motor )
p_encoder = PidEncoder( encoder )

pid = wpilib.SmartDashboard.SendablePIDController( 0.2, 0.0, 0.0, p_encoder, p_motor )

sd = wpilib.SmartDashboard.SmartDashboard.GetInstance()
sd.PutData( "PID1", pid )


class MyRobot(wpilib.SimpleRobot):

    def __init__(self):
        '''Constructor'''
        
        wpilib.SimpleRobot.__init__(self)
        encoder.SetPIDSourceParameter( wpilib.Encoder.kRate )
        encoder.SetDistancePerPulse( 1/300.0 )      # this is for the small wheel we have
        encoder.Start()
        
            
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
                    pid.Enable()
                    enabled = True
                
                pid.SetSetpoint( stick1.GetY() )
                
            else:
                if enabled:
                    pid.Disable()
                    enabled = False
                    motor.Set(0.0)
            
            
            if timer.HasPeriodPassed( .5 ):
                print( "Encoder: %s; SP: %s: Motor: %s" % (encoder.GetRate(), pid.GetSetpoint(), p_motor.value ) )
            
            wpilib.Wait(0.04)

        
def run():
    '''This function must be present for the robot to start'''
    robot = MyRobot()
    robot.StartCompetition()
    
    # must return a value to work with the tester program
    return robot
