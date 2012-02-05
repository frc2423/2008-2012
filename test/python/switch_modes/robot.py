
#import rpdb2

import wpilib

stick1 = wpilib.Joystick(1)
stick2 = wpilib.Joystick(2)

# PID constants
ARM_P = 360.0
ARM_I = .5
ARM_D = 0.0


ENCODER_TURNS_PER_REVOLUTION = 360

class MyRobot(wpilib.SimpleRobot):

    def __init__(self):
        print("MyRobot::__init__()")
        
        self.vertical_motor = wpilib.CANJaguar( 10 )
        
        # set parameters
        self.vertical_motor.ConfigEncoderCodesPerRev( ENCODER_TURNS_PER_REVOLUTION )
        self.vertical_motor.ConfigNeutralMode( wpilib.CANJaguar.kNeutralMode_Brake )
        
        self.arm_stick = stick2

    def Disabled(self):
        print("MyRobot::Disabled()")
        while self.IsDisabled():
            wpilib.Wait(0.01)

    def Autonomous(self):
        self.GetWatchdog().SetEnabled(False)
        while self.IsAutonomous() and self.IsEnabled():
            wpilib.Wait(0.01)
            
    def set_jaguar_control_mode(self, mode):
        '''Sets the control mode for the Jaguar'''
    
        if self.vertical_motor.GetControlMode() != mode:
        
            print( "Trying to change the mode to %d" % int(mode) )
        
            self.vertical_motor.ChangeControlMode(mode)
        
            if mode == wpilib.CANJaguar.kPosition:
                
                self.vertical_motor.SetPositionReference( wpilib.CANJaguar.kPosRef_QuadEncoder )
                self.vertical_motor.ConfigEncoderCodesPerRev( ENCODER_TURNS_PER_REVOLUTION )
                self.vertical_motor.SetPID( ARM_P, ARM_I, ARM_D )
        
                self.vertical_motor.EnableControl()

    def OperatorControl(self):
        print("MyRobot::OperatorControl()")
        
        dog = self.GetWatchdog()
        dog.SetEnabled(True)
        dog.SetExpiration(0.25)
        
        timer = wpilib.Timer()
        timer.Start()
        
        holding = False
    
        set_position = 0.0

        # main loop
        while self.IsOperatorControl() and self.IsEnabled():
        
            # always feed the watchdog!
            dog.Feed()
            
            arm_trigger = self.arm_stick.GetTrigger()
            
            # make sure that if you press the hold button while holding
            # the trigger (manual arm mode), then you should not allow
            # manual control until you let go of the trigger again
            if not holding:
                if arm_trigger:
                    self.set_jaguar_control_mode( wpilib.CANJaguar.kPercentVbus )
                    set_position = self.arm_stick.GetY()
                else:
                    set_position = 0.0
            else:
                if arm_trigger == False:
                    holding = False
            
            if holding == False and self.arm_stick.GetTop():
                self.set_jaguar_control_mode( wpilib.CANJaguar.kPosition )
                set_position = self.vertical_motor.GetPosition()
                holding = True
                
            if self.arm_stick.GetRawButton(7):
                set_position = -2.0
                
            self.vertical_motor.Set( set_position )
        
            if timer.HasPeriodPassed(1.0):
                print( "Encoder: %f, mode: %d; Set to %f; IsAlive: %s" % \
                    (   self.vertical_motor.GetPosition(), 
                        int( self.vertical_motor.GetControlMode() ), 
                        self.vertical_motor.Get(),
                        self.vertical_motor.IsAlive()
                    ))
                print( "P: %f I: %f D: %f" % \
                    (   self.vertical_motor.GetP(),
                        self.vertical_motor.GetI(),
                        self.vertical_motor.GetD()
                    ))
    
            wpilib.Wait(0.04)

        
def run():
    #rpdb2.start_embedded_debugger()

    '''This function must be present for the robot to start'''
    robot = MyRobot()
    robot.StartCompetition()




