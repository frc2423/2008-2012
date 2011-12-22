
#import rpdb2

import wpilib

stick1 = wpilib.Joystick(1)
stick2 = wpilib.Joystick(2)

THUMP_MAX_POSITION = .82
THUMP_MIN_POSITION = .50

# PID constants
ARM_P = 1500.0
ARM_I = 0.5
ARM_D = 0.0


class MyRobot(wpilib.SimpleRobot):

    def __init__(self):
        
        self.vertical_motor = wpilib.CANJaguar(10)
        self.thump_motor = wpilib.CANJaguar(11)
        
        self.position = False
        
        self.thump_motor.SetPositionReference( wpilib.CANJaguar.kPosRef_Potentiometer )
        self.thump_motor.ConfigPotentiometerTurns( 1 )
        
        # self.thump_motor = wpilib.CANJaguar(11, wpilib.CANJaguar.kPosition )
        # self.thump_motor.SetPID( .95, 0.0, 0.0 )
        
        
        # self.thump_motor.EnableControl()

        
        

    def Disabled(self):
        
        print("MyRobot::Disabled()")
    
        while self.IsDisabled():
            wpilib.Wait(0.01)

    def Autonomous(self):
        self.GetWatchdog().SetEnabled(False)
        while self.IsAutonomous() and self.IsEnabled():
            wpilib.Wait(0.01)
            
            
    def toggle_mode(self):
    
        if self.thump_motor.GetControlMode() == wpilib.CANJaguar.kPosition:
            self.set_jaguar_control_mode( wpilib.CANJaguar.kPercentVbus )
        else:
            self.set_jaguar_control_mode( wpilib.CANJaguar.kPosition )
        
    def set_jaguar_control_mode(self, mode):
        '''Sets the control mode for the Jaguar'''
    
        if self.thump_motor.GetControlMode() != mode:
        
            print( "Trying to change the mode to %d" % int(mode) )
        
            self.thump_motor.ChangeControlMode(mode)
        
            if mode == wpilib.CANJaguar.kPosition:
                
                self.position = True
                
                self.thump_motor.SetPositionReference( wpilib.CANJaguar.kPosRef_Potentiometer )
                self.thump_motor.ConfigPotentiometerTurns( 1 )
                
                #self.thump_motor.ConfigSoftPositionLimits( THUMP_MIN_POSITION, THUMP_MAX_POSITION )
                
                #self.thump_motor.ConfigEncoderCodesPerRev( ENCODER_TURNS_PER_REVOLUTION )
                #self.thump_motor.SetPositionReference( wpilib.CANJaguar.kPosRef_QuadEncoder )
                self.thump_motor.SetPID( ARM_P, ARM_I, ARM_D )
        
                self.thump_motor.EnableControl()
            else:
                self.position = False
                self.thump_motor.DisableControl()
            
    def _translate_z(self, z):
    
        # Xmax - (Ymax - Y)( (Xmax - Xmin) / (Ymax - Ymin) )
        return THUMP_MAX_POSITION - ((1 - z)*( (THUMP_MAX_POSITION - THUMP_MIN_POSITION) / 1 ) )

        
    def OperatorControl(self):
    
        print("MyRobot::OperatorControl()")
    
        dog = self.GetWatchdog()
        dog.SetEnabled(True)
        dog.SetExpiration(0.25)
        
        timer = wpilib.Timer()
        timer.Start()

        # main loop
        while self.IsOperatorControl() and self.IsEnabled():
        
            # always feed the watchdog!
            dog.Feed()
            
            # vertical arm code
            if self.position:
                self.thump_motor.Set( self._translate_z( (stick2.GetZ() + 1.0) / 2.0 ) )
            else:
                if stick2.GetTrigger():
                    self.thump_motor.Set( stick2.GetY() )
                else:
                    self.thump_motor.Set( 0.0 )
            
            #self.thump_motor.Set( stick2.GetZ() )
                
            if stick2.GetTop() and not on:
                on = True
                self.toggle_mode()
            elif not stick2.GetTop():
                on = False
                
            if timer.HasPeriodPassed( 1.0 ):
                print( "Thump position: %f; Current: %f; Voltage: %f; Set: %f" % ( \
                    self.thump_motor.GetPosition(),
                    self.thump_motor.GetOutputCurrent(),
                    self.thump_motor.GetOutputVoltage(),
                    self.thump_motor.Get() ))
                
            #if stick1.GetTrigger():
            #    self.thump_motor.Set( stick1.GetY() )
            #else:
            #    self.thump_motor.Set( 0.0 )
                
            # self.thump_motor.Set( self._translate_z( stick1.GetZ() ))
            
    
            wpilib.Wait(0.04)

        
def run():
    #rpdb2.start_embedded_debugger()

    '''This function must be present for the robot to start'''
    robot = MyRobot()
    robot.StartCompetition()




