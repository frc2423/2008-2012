

try:
    import wpilib
    from wpilib.NetworkTables import NetworkTable
    from wpilib.NetworkTables import NetworkTableChangeListener
    
except ImportError:

    import fake_wpilib as wpilib
   
class MyRobot(wpilib.SimpleRobot):
    
    kTableName = "TrackingData"
    kDistance = "distance"
    kSonarDistance = "sonar_distance"
    kAngleSusan = "angle_susan"
    
    def __init__(self):
        '''Constructor'''
        
        wpilib.SimpleRobot.__init__(self)
        self.table = NetworkTable.GetTable(MyRobot.kTableName)
        self.distance = 0.0
        self.sonar = 0.0
        self.angle = 0.0
     
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
        
        timer = wpilib.Timer()
        timer.Start()
        
        while self.IsOperatorControl() and self.IsEnabled():
            '''Begin transaction, get all data'''
            self.table.BeginTransaction()
            self.distance = self.table.GetDouble(MyRobot.kDistance)
            self.sonar = self.table.GetDouble(MyRobot.kSonarDistance)
            self.angle = self.table.GetDouble(MyRobot.kAngleSusan)
            print("The distance is: " + str(self.distance))
            print("The sonar distance is: " + str(self.sonar))
            print("The angle susan is:" + str(self.angle))
            self.table.EndTransaction()
            wpilib.Wait( 0.05 )
        
        pass
def run():
    '''This function must be present for the robot to start'''
    robot = MyRobot()
    robot.StartCompetition()
    
    # must return a value to work with the tester program
    return robot