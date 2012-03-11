'''
    Vision System Tests

'''

# print something out so we know what code is running
print( "Loading: Kwarqs2012 Vision program" )

try:
    import wpilib
except ImportError:
    import fake_wpilib as wpilib

    
class MyRobot(wpilib.SimpleRobot):

    def __init__(self):
        '''Constructor'''
        

            
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
        self.table = NetworkTable()
        
        self.table = self.table.GetSubTable("TrackingData")
        
        dog = self.GetWatchdog()
        dog.SetEnabled(True)
        dog.SetExpiration(0.25)
        
        timer = wpilib.Timer()
        timer.Start()

        # main loop
        while self.IsOperatorControl() and self.IsEnabled():
            
            dog.Feed()
        
            distance = self.table.GetDouble("distance")
            
            if timer.HasPeriodPassed(1.0):
                print("Distance is : " + str(distance))
def run():
    '''This function must be present for the robot to start'''
    robot = MyRobot()
    robot.StartCompetition()
    
    # must return a value to work with the tester program
    return robot



