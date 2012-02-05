
#import rpdb2

import wpilib

stick1 = wpilib.Joystick(1)
stick2 = wpilib.Joystick(2)

THUMP_MAX_POSITION = 1
THUMP_MIN_POSITION = 0


# PID constants
ARM_P = 360.0
ARM_I = 0.0
ARM_D = 0.0

ENCODER_TURNS_PER_REVOLUTION = 360

class MyRobot(wpilib.SimpleRobot):

    def __init__(self):
        self.vertical_motor = wpilib.CANJaguar(10)

    def Disabled(self):
        
        print("MyRobot::Disabled()")
    
        while self.IsDisabled():
            wpilib.Wait(0.01)

    def Autonomous(self):
        self.GetWatchdog().SetEnabled(False)
        while self.IsAutonomous() and self.IsEnabled():
            wpilib.Wait(0.01)
            
            
    def _translate_z(self, z):
    
        # Xmax - (Ymax - Y)( (Xmax - Xmin) / (Ymax - Ymin) )
        return THUMP_MAX_POSITION - ((1 - z)*( (THUMP_MAX_POSITION - THUMP_MIN_POSITION) / 2 ) )
        
        
    def toggle_mode(self):
    
        if self.vertical_motor.GetControlMode() == wpilib.CANJaguar.kPosition:
            self.set_jaguar_control_mode( wpilib.CANJaguar.kPercentVbus )
        else:
            self.set_jaguar_control_mode( wpilib.CANJaguar.kPosition )
        
    def set_jaguar_control_mode(self, mode):
        '''Sets the control mode for the Jaguar'''
    
        if self.vertical_motor.GetControlMode() != mode:
        
            print( "Trying to change the mode to %d" % int(mode) )
        
            self.vertical_motor.ChangeControlMode(mode)
        
            if mode == wpilib.CANJaguar.kPosition:
                
                self.vertical_motor.ConfigEncoderCodesPerRev( ENCODER_TURNS_PER_REVOLUTION )
                self.vertical_motor.SetPositionReference( wpilib.CANJaguar.kPosRef_QuadEncoder )
                self.vertical_motor.SetPID( ARM_P, ARM_I, ARM_D )
        
                self.vertical_motor.EnableControl()
            else:
                self.vertical_motor.DisableControl()

        
    def OperatorControl(self):
    
        print("MyRobot::OperatorControl()")
    
        dog = self.GetWatchdog()
        dog.SetEnabled(True)
        dog.SetExpiration(0.25)
        
        timer = wpilib.Timer()
        timer.Start()
        
        on = False

        # main loop
        while self.IsOperatorControl() and self.IsEnabled():
        
            # always feed the watchdog!
            dog.Feed()
            
            # vertical arm code
            if stick2.GetTrigger():
                self.vertical_motor.Set( stick2.GetY() )
            else:
                self.vertical_motor.Set( 0.0 )
                
            if stick2.GetTop() and not on:
                on = True
                self.toggle_mode()
            elif not stick2.GetTop():
                on = False
                
            if timer.HasPeriodPassed( 1.0 ):
                print( "Position: %f; Current: %f; Voltage: %f; Set: %f" % ( \
                    self.vertical_motor.GetPosition(),
                    self.vertical_motor.GetOutputCurrent(),
                    self.vertical_motor.GetOutputVoltage(),
                    self.vertical_motor.Get() ))
            
            wpilib.Wait(0.04)

        
def run():
    #rpdb2.start_embedded_debugger()

    '''This function must be present for the robot to start'''
    robot = MyRobot()
    robot.StartCompetition()




