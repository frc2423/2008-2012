import wpilib

# The vertical positions for the arm
ARM_1 = 1
ARM_2 = 2
ARM_3 = 3
ARM_4 = 4
ARM_5 = 5
ARM_6 = 6

class Arm(object):
    
    def __init__(self):
        '''Constructor'''
        pass
    
    def set_vertical_position(self, y):
        '''Tell the arm to go to a specific position'''
        pass
        
    def manual_vertical_control(self, y):
        '''Tell the arm to go up or down'''
        pass
        
    def deploy_tube(self):
        '''Place the tube on the peg'''
        pass
        
    def retrieve_tube(self):
        '''Pick up the tube'''
        pass
        
    def do_control_loop(self):
        '''Motor control'''
        pass

    def arm_is_in_position(self):
        '''Check if placement of arm is correct'''
        pass
        
        