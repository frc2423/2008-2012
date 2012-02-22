
# directory of robot.py, relative to this file
robot_path = '../../source'



class Test(object):
    loopNum = 0
    stick = None

    def __init__(self, robot_module, myrobot):
        self.robot_module = robot_module
        self.myrobot = myrobot
        self.loop_count = 0
        
    def IsOperatorControl(self):
        '''
            Print function should be changed in robot.py to print correctly 
            right after setting values.
        '''    
            
        stick = self.robot_module.stick2
        stick.x = ((Test.loopNum)/10)
        print(stick.GetX())
        Test.loopNum += 1
        if Test.loopNum < 10:
            return True
        Test.loopNum = 0
        stick.x  = -((Test.loopNum)/10)
        Test.loopNum += 1
        if Test.loopNum < 10:
            return False
        
def run_tests( robot_module, myrobot ):

    test = Test( robot_module, myrobot )
    dIn = robot_module.driveStation.digital_in
    joy = robot_module.stick1.buttons
    myrobot.enabled = True
    myrobot.on_IsOperatorControl = test.IsOperatorControl
    dIn[4] = True
    print("digital in: " + str(dIn))
    robot_module.stick1.buttons[7] = True
    print("stick 1: " + str(joy))
    myrobot.OperatorControl()


