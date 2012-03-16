
# directory of robot.py, relative to this file
robot_path = '../../source'

class Test(object):

    def __init__(self, robot_module, myrobot):
        self.robot_module = robot_module
        self.myrobot = myrobot
        self.loop_count = 0
        
    def IsOperatorControl(self, tm):
        self.loop_count += 1
        return not self.loop_count == 1000
        


def run_tests( robot_module, myrobot ):

    test = Test( robot_module, myrobot )

    robot_module.wpilib._print_components()
    
    myrobot.enabled = True
    myrobot.on_IsOperatorControl = test.IsOperatorControl
    
    myrobot.OperatorControl()


