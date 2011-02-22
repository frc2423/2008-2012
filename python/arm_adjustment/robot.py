
#import rpdb2

import wpilib

stick1 = wpilib.Joystick(1)
stick2 = wpilib.Joystick(2)

THUMP_MAX_POSITION = 1
THUMP_MIN_POSITION = 0


class MyRobot(wpilib.SimpleRobot):

    def __init__(self):
        
        self.vertical_motor = wpilib.CANJaguar(10)
        self.thump_motor = wpilib.CANJaguar(11)
        
        #self.thump_motor = wpilib.CANJaguar(11, wpilib.CANJaguar.kPosition )
        # self.thump_motor.SetPID( .95, 0.0, 0.0 )
        
        self.thump_motor.SetPositionReference( wpilib.CANJaguar.kPosRef_Potentiometer )
        self.thump_motor.ConfigPotentiometerTurns( 1 )
        
        # self.thump_motor.ConfigSoftPositionLimits( THUMP_MIN_POSITION, THUMP_MAX_POSITION )
        # self.thump_motor.EnableControl()

        
        

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
            if stick2.GetTrigger():
                self.vertical_motor.Set( stick2.GetY() )
            else:
                self.vertical_motor.Set( 0.0 )
                
            if timer.HasPeriodPassed( 1.0 ):
                print( "Thump position: %f; Current: %f; Voltage: %f; Set: %f" % ( \
                    self.thump_motor.GetPosition(),
                    self.thump_motor.GetOutputCurrent(),
                    self.thump_motor.GetOutputVoltage(),
                    self.thump_motor.Get() ))
                
            if stick1.GetTrigger():
                self.thump_motor.Set( stick1.GetY() )
            else:
                self.thump_motor.Set( 0.0 )
                
            # self.thump_motor.Set( self._translate_z( stick1.GetZ() ))
            
    
            wpilib.Wait(0.04)

        
def run():
    #rpdb2.start_embedded_debugger()

    '''This function must be present for the robot to start'''
    robot = MyRobot()
    robot.StartCompetition()




