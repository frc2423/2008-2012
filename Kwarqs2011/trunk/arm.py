import wpilib

# The vertical positions for the arm
ARM_1 = 1
ARM_2 = 2
ARM_3 = 3
ARM_4 = 4
ARM_5 = 5
ARM_6 = 6

# Tube states
TUBE_STATE_DEPLOY = 1
TUBE_STATE_RETRIEVE = 2
TUBE_STATE_OFF = 3

class Arm(object):
    
    def __init__(self):
        '''Constructor'''
        self.vertical_motor = wpilib.CANJaguar()
        self.scooper_motor = wpilib.Relay(1)
        self.thump_motor = wpilib.CANJaguar()
        self.thump_button = wpilib.DigitalInput(1)
        
        self.tube_state = TUBE_STATE_OFF
    
    def set_vertical_position(self, y):
        '''Tell the arm to go to a specific position'''
        pass
        
    def manual_vertical_control(self, y):
        '''Tell the arm to go up or down'''
        pass
        
    def arm_is_in_position(self):
        '''Check if placement of arm is correct'''
        pass
        
    def deploy_tube(self):
        '''Place the tube on the peg'''
        self.tube_state = TUBE_STATE_DEPLOY
        
    def retrieve_tube(self):
        '''Pick up the tube'''
        self.tube_state = TUBE_STATE_RETRIEVE
        
    def do_control_loop(self):
        '''Motor control'''
    
        # Telling the scooper what to do
        if self.tube_state == TUBE_STATE_DEPLOY:
            self.scooper_motor.Set(wpilib.Relay.kForward)
        elif self.tube_state == TUBE_STATE_RETRIEVE:
            self.scooper_motor.Set(wpilib.Relay.kReverse)
        elif self.tube_state == TUBE_STATE_OFF:
            self.scooper_motor.Set(wpilib.Relay.kOff)
        
        # Make sure to turn the scooper off each time so it doesn't run continuously
        self.tube_state = TUBE_STATE_OFF
        
        # Make the thump motor deploy the arm
        if self.thump_button.Get():
            self.thump_motor.Set(1)
        else:
            self.thump_motor.Set(0)
        