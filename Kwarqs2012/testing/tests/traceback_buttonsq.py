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

    def IsOperatorControl(self):
        #global loop_count
        self.loop_count = self.loop_count + 1
        print(self.loop_count)
        
        
        if self.loop_count == 1:
            self.robot_module.driveStation.digital_in[0] = True
        
        
        if self.loop_count == 2:
            self.robot_module.driveStation.digital_in[0] = False
            self.robot_module.driveStation.digital_in[1] = True
        
        
        if self.loop_count == 3:
            self.robot_module.driveStation.digital_in[1] = False
            self.robot_module.driveStation.digital_in[2] = True
        
        
        if self.loop_count == 4:
            self.robot_module.driveStation.digital_in[2] = False  
            self.robot_module.driveStation.digital_in[3] = True 
        
        if self.loop_count == 5:
            self.robot_module.driveStation.digital_in[3] = False   
            self.robot_module.driveStation.digital_in[4] = True  
        
        if self.loop_count == 6:
            self.robot_module.driveStation.digital_in[4] = False  
            self.robot_module.driveStation.digital_in[5] = True 
            
        if self.loop_count == 7:
            self.robot_module.driveStation.digital_in[5] = False  
            self.robot_module.driveStation.digital_in[6] = True     
        
        if self.loop_count == 8:
            self.robot_module.driveStation.digital_in[6] = False   
            self.robot_module.stick1.buttons[0] = True    
            
        if self.loop_count == 9:
            self.robot_module.stick1.buttons[0] = False 
            self.robot_module.stick1.buttons[1] = True        
                
            
        '''   
        if self.loop_count == 2:
            self.robot_module.stick1.buttons[1] = False
            self.robot_module.stick1.buttons[2] = True
            
        '''    
        if self.loop_count == 10:
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