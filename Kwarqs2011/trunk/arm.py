import wpilib

# The vertical positions for the arm
ARM_1 = 1
ARM_2 = 2
ARM_3 = 3
ARM_4 = 4
ARM_5 = 5
ARM_6 = 6

ARM_TOLERANCE = 45
ARM_POSITION_MIN = -5000
ARM_POSITION_MAX = 5000
ENCODER_TURNS_PER_REVOLUTION = 1024

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
        
        # configure the vertical motor to be in the correct mode and such
        self.vertical_motor.SetPositionReference( wpilib.CANJaguar.kPosRef_QuadEncoder )
        self.vertical_motor.ConfigEncoderCodesPerRev( ENCODER_TURNS_PER_REVOLUTION )
        self.vertical_motor.ConfigSoftPositionLimits( ARM_POSITION_MIN, ARM_POSITION_MAX )
        
        # Manual motor value of vertical motor
        self.vertical_motor_value = None
        
        # Position we want the arm to be in
        self.vertical_motor_position = None
        
        self.tube_state = TUBE_STATE_OFF
    
    def set_vertical_position(self, y):
        '''Tell the arm to go to a specific position'''
        self.vertical_motor_position = y
        
    def manual_vertical_control(self, y):
        '''Tell the arm to go up or down'''
        self.vertical_motor_value = y
        
    def arm_is_in_position(self):
        '''Check if placement of arm is correct'''
        
        if self.vertical_motor_value is not None:
            # automated mode
            position = self.vertical_motor.GetPosition()
            
            if position <= self.vertical_motor_value + ARM_TOLERANCE and position >= self.vertical_motor_value - ARM_TOLERANCE:
                return True
            else:
                return False
                
        else:
            # We're in manual mode here -- we assume that if the human
            # has let go of the trigger ( manual_vertical_control hasn't
            # been called ), then we must be in the correct position
            return self.vertical_motor_value == None
        
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
        
        # If it is in Manual mode...
        if self.vertical_motor_value is not None:
        
            # ... and if the Jaguar is not in the correct control mode...
            if self.vertical_motor.GetControlMode() != wpilib.CANJaguar.kPercentVbus:
            
                # ... set the output mode.
                self.vertical_motor.SetControlMode(wpilib.CANJaguar.kPercentVbus)
            
            # Set motor value
            self.vertical_motor.Set(self.vertical_motor_value)
            
            # Turn off Manual Mode
            self.vertical_motor_value = None
            
        # If it is not in Manual mode...
        else:
            if self.vertical_position is not None:
                if self.vertical_motor.GetControlMode() != wpilib.CANJaguar.kPosition: 
                    self.vertical_motor.SetControlMode(wpilib.CANJaguar.kPosition)
                    
                self.vertical_motor.Set(self.vertical_motor_position)
                self.vertical_motor_position == None   
                    
            else:
                if self.vertical_motor.GetControlMode() == wpilib.CANJaguar.kPercentVbus:
                    self.vertical_motor.Set(0)
                
                    
                    
                    
                    
                    
                    