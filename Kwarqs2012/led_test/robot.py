

try:
    import wpilib
    import wpilib.SmartDashboard
    from operator_leds import OperatorLEDs
    from robot_widget import RobotWidget
except ImportError:

    import fake_wpilib as wpilib
    import fake_wpilib.SmartDashboard

    import imp
    import os
    
    fname = os.path.normpath( os.path.dirname(os.path.abspath(__file__)) + '/../source/operator_leds.py' )
    operator_leds = imp.load_source( 'operator_leds', fname )
    OperatorLEDs = operator_leds.OperatorLEDs
    
    fname = os.path.normpath( os.path.dirname(os.path.abspath(__file__)) + '/../source/robot_widget.py' )
    robot_widget = imp.load_source( 'robot_widget', fname )
    RobotWidget = robot_widget.RobotWidget



MANUAL_LEDS = False

widget = RobotWidget('Widget')

stick1 = wpilib.Joystick(1)
    
class MyRobot(wpilib.SimpleRobot):

    def __init__(self):
        '''Constructor'''
        
        wpilib.SimpleRobot.__init__(self)
        
        self.ds = wpilib.DriverStation.GetInstance()
        
        self.sd = wpilib.SmartDashboard.SmartDashboard.GetInstance()
        self.eio = wpilib.DriverStation.GetInstance().GetEnhancedIO()
        
        self.eio.SetDigitalConfig( 10, wpilib.DriverStationEnhancedIO.kOutput )
        
        self.eio.SetDigitalConfig( 2, wpilib.DriverStationEnhancedIO.kInputPullUp )
        self.eio.SetDigitalConfig( 4, wpilib.DriverStationEnhancedIO.kInputPullUp )
        self.eio.SetDigitalConfig( 8, wpilib.DriverStationEnhancedIO.kInputPullUp )
        self.eio.SetDigitalConfig( 12, wpilib.DriverStationEnhancedIO.kInputPullUp )
        self.eio.SetDigitalConfig( 16, wpilib.DriverStationEnhancedIO.kInputPullUp )
        
        if MANUAL_LEDS:
            self.sd.PutBoolean( "Enable 1", False ) 
            self.sd.PutBoolean( "Enable 2", False ) 
            
            self.sd.PutBoolean( "D0 - 0", False )
            self.sd.PutBoolean( "D0 - 1", False )
            self.sd.PutBoolean( "D0 - 2", False )
            self.sd.PutBoolean( "D0 - 3", False )
            
            self.sd.PutBoolean( "D1 - 0", False )
            self.sd.PutBoolean( "D1 - 1", False )
            self.sd.PutBoolean( "D1 - 2", False )
            self.sd.PutBoolean( "D1 - 3", False )
            
            self.eio.SetDigitalConfig( OperatorLEDs.G_EN[0], wpilib.DriverStationEnhancedIO.kOutput )
            self.eio.SetDigitalConfig( OperatorLEDs.G_EN[1], wpilib.DriverStationEnhancedIO.kOutput )
            
            self.eio.SetDigitalConfig( OperatorLEDs.D0[0], wpilib.DriverStationEnhancedIO.kOutput )
            self.eio.SetDigitalConfig( OperatorLEDs.D0[1], wpilib.DriverStationEnhancedIO.kOutput )
            self.eio.SetDigitalConfig( OperatorLEDs.D0[2], wpilib.DriverStationEnhancedIO.kOutput )
            self.eio.SetDigitalConfig( OperatorLEDs.D0[3], wpilib.DriverStationEnhancedIO.kOutput )
            
            self.eio.SetDigitalConfig( OperatorLEDs.D1[0], wpilib.DriverStationEnhancedIO.kOutput )
            self.eio.SetDigitalConfig( OperatorLEDs.D1[1], wpilib.DriverStationEnhancedIO.kOutput )
            self.eio.SetDigitalConfig( OperatorLEDs.D1[2], wpilib.DriverStationEnhancedIO.kOutput )
            self.eio.SetDigitalConfig( OperatorLEDs.D1[3], wpilib.DriverStationEnhancedIO.kOutput )
        
        else:
            self.operator_leds = OperatorLEDs()
            self.sd.PutInt( "Digit1", 0 )
            self.sd.PutInt( "Digit2", 99 )
            
        self.sd.PutBoolean( "LED", False )
            
    def Disabled(self):
        
        print("MyRobot::Disabled()")
    
        while self.IsDisabled():
            if MANUAL_LEDS == False and self.ds.IsNewControlData():
                self.operator_leds.InDisabled()
                
            wpilib.Wait(0.01)
        
    
    def OperatorControl(self):
        '''Called during Teleoperated Mode'''
    
        print("MyRobot::OperatorControl()")
        
        timer = wpilib.Timer()
        timer.Start()
        
        if not MANUAL_LEDS:
            digit1 = self.operator_leds.GetDigitGroup(1)
            digit2 = self.operator_leds.GetDigitGroup(2)
        
        while self.IsOperatorControl() and self.IsEnabled():

            if MANUAL_LEDS:
                self.eio.SetDigitalOutput( OperatorLEDs.G_EN[0], self.sd.GetBoolean( "Enable 1" ) )
                self.eio.SetDigitalOutput( OperatorLEDs.G_EN[1], self.sd.GetBoolean( "Enable 2" ) )
                
                self.eio.SetDigitalOutput( OperatorLEDs.D0[0], self.sd.GetBoolean( "D0 - 0" ) )
                self.eio.SetDigitalOutput( OperatorLEDs.D0[1], self.sd.GetBoolean( "D0 - 1" ) )
                self.eio.SetDigitalOutput( OperatorLEDs.D0[2], self.sd.GetBoolean( "D0 - 2" ) )
                self.eio.SetDigitalOutput( OperatorLEDs.D0[3], self.sd.GetBoolean( "D0 - 3" ) )
                
                self.eio.SetDigitalOutput( OperatorLEDs.D1[0], self.sd.GetBoolean( "D1 - 0" ) )
                self.eio.SetDigitalOutput( OperatorLEDs.D1[1], self.sd.GetBoolean( "D1 - 1" ) )
                self.eio.SetDigitalOutput( OperatorLEDs.D1[2], self.sd.GetBoolean( "D1 - 2" ) )
                self.eio.SetDigitalOutput( OperatorLEDs.D1[3], self.sd.GetBoolean( "D1 - 3" ) )
            
            else:
                d1 = ((stick1.GetY() + 1.0) / 2.0) * 100.0
                d2 = ((stick1.GetZ() + 1.0) / 2.0) * 100.0
                
                digit1.Set( d1 )
                digit2.Set( d2 )
                
                if self.ds.IsNewControlData():
                    self.operator_leds.Update()
                    
                    
            widget.SetChamber( self.eio.GetDigital(2) )
            widget.SetMiddleVirtual( self.eio.GetDigital(4) )
            widget.SetFeederTop( self.eio.GetDigital(8) )
            widget.SetFeederVirtual( self.eio.GetDigital(12) )
            widget.SetFeederLow( self.eio.GetDigital(16) )
            
            self.eio.SetDigitalOutput( 10, self.sd.GetBoolean( "LED" ) )
            
            wpilib.Wait( 0.05 )
    
    
def run():
    '''This function must be present for the robot to start'''
    robot = MyRobot()
    robot.StartCompetition()
    
    # must return a value to work with the tester program
    return robot
