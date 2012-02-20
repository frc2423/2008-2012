'''
This program is meant to test the logic of our automatic functions

'''

try:
    import wpilib
except ImportError:
    import fake_wpilib as wpilib
# directory of robot.py, relative to this file

robot_path = '../../source'


class Test(object):

    def __init__(self, robot_module, myrobot):
        self.loop_count = 0
        self.robot_module = robot_module
        self.myrobot = myrobot
        
        
        '''
        These are our instances of each component
        This is for easy access for testing 
        '''
        self.feeder = self.robot_module.feeder
        self.chamber = self.robot_module.chamber
        
        '''
        This is to easily access our instances of IRsensors
        To test what happens when IRsensors register balls
        '''
        self.topFeedIRSens = self.feeder.topFeedIRSens.irSensor
        self.botFeedIRSens = self.feeder.botFeedIRSens.irSensor
        self.chambIRSens = self.chamber.chambIRSens.irSensor
        

    def IsOperatorControl(self):
        #global loop_count
        self.loop_count = self.loop_count + 1
        print(self.loop_count)
        #print( self.robot_module.feeder.IsReady() )
        
        # Template to check if a ball is there - 
        #self.topFeedIRSens.IsBallSet
        
        
        
        #1 ball at bottom of feeder
        #expect the motor to continue running
        if self.loop_count <= 5:
        
            self.botFeedIRSens.voltage = 2.5
            print (self.topFeedIRSens.IsBallSet)
            #self.irSensor.GetVoltage()
        
        
        #1 ball at top of feeder
        #expect the motor to stop running and for the feeder to be ready
        if self.loop_count > 5 and self.loop_count <= 10:
        
            self.botFeedIRSens.voltage = 0.3
            self.topFeedIRSens.voltage = 2.5
            feeder.IsReady
            print (self.topFeedIRSens.IsBallSet)
            #self.irSensor.GetVoltage()
        
        
        #1 ball at top and bottom of feeder
        #expect the motor to stop running and for the feeder to be ready
        if self.loop_count > 10 and self.loop_count <= 15:
        
            self.botFeedIRSens.voltage = 2.5
            self.topFeedIRSens.voltage = 2.5
            feeder.IsReady
            print (self.topFeedIRSens.IsBallSet)
            #self.irSensor.GetVoltage()
        
        
        #1 ball at top of feeder and 1 in chamber
        #expect the motor to stop running and for the feeder AND chamber to be ready
        if self.loop_count > 15 and self.loop_count <= 20:
        
            self.botFeedIRSens.voltage = 0.3
            self.chambIRSens.voltage = 2.5
            self.topFeedIRSens.voltage = 2.5
            feeder.IsReady
            print (self.topFeedIRSens.IsBallSet)
            #self.irSensor.GetVoltage()
        
        #both feeder and chamber full
        #expect both feeder and chamber motors to be off. I feeder is on we could have 4 balls.
        if self.loop_count > 20 and self.loop_count <= 25:
        
            self.botFeedIRSens.voltage = 2.5
            self.chambIRSens.voltage = 2.5
            self.topFeedIRSens.voltage = 2.5
            feeder.IsReady
            print (self.topFeedIRSens.IsBallSet)
            #self.irSensor.GetVoltage()
        
        
        '''
        if self.loop_count == 1:
            
        
        
        
            
        '''

        '''    
        if self.loop_count == 2:
            self.robot_module.stick1.buttons[1] = False
            self.robot_module.stick1.buttons[2] = True
        '''    
           
        if self.loop_count == 26:
            return False
           
        return True

    
        
def run_tests( robot_module, myrobot ):

    test = Test( robot_module, myrobot )

    myrobot.enabled = True
    myrobot.on_IsOperatorControl = test.IsOperatorControl
    
    myrobot.OperatorControl()

    
    
    
    # TODO: Actually do something here
    '''
    loop_count = 0
    
    while loop_count < 10:
        loop_count += 1
        
        if loop_count == 1:
            wpilib.Joystick.GetRawButton(1)
        
        if loop_count == 2:
            wpilib.buttons[1]
        
        if loop_count == 3:
            wpilib.self.buttons[4]
        
        if loop_count == 4:
            wpilib.self.buttons[5]
        
        if loop_count == 5:
            wpilib.self.buttons[6]
    '''