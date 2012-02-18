try:
    import wpilib
except ImportError:
    import fake_wpilib as wpilib
# directory of robot.py, relative to this file


robot_path = '../../source'


class Test(object):
    def __init__(self):
        self.loop_count = 0

    def IsOperatorControl(self):
        #global loop_count
        self.loop_count = loop_count + 1
        print(self.loop_count)
        
        if self.loop_count == 10:
            return False
            
        return True


    def run_tests( self, robot_module, myrobot ):

        myrobot.enabled = True
        myrobot.on_IsOperatorControl = IsOperatorControl
        
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
            

       

