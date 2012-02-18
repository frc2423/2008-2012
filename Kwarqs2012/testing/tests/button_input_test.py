try:
    import wpilib
except ImportError:
    import fake_wpilib as wpilib
# directory of robot.py, relative to this file
import robot
robot_path = '../../source'


def run_tests( robot_module, myrobot ):

    # TODO: Actually do something here
    loop_count = 0
    
    while loop_count < 10:
        loop_count += 1
        
        if loop_count == 1:
            wpilib.self.buttons[0]
        
         if loop_count == 2:
            wpilib.self.buttons[1]
        
         if loop_count == 3:
            wpilib.self.buttons[4]
        
         if loop_count == 4:
            wpilib.self.buttons[5]
        
         if loop_count == 5:
            wpilib.self.buttons[6]
    

   

