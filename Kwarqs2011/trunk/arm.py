import wpilib
from util import *

# The vertical positions for the arm
ARM_1 = .1       #   2
ARM_2 = .2       # 1
ARM_3 = .3       #   4
ARM_4 = .4       # 3
ARM_5 = .5       #   6
ARM_6 = .6       # 5

ARM_TOLERANCE = 45
ARM_POSITION_MIN = -5000
ARM_POSITION_MAX = 5000
ENCODER_TURNS_PER_REVOLUTION = 360

CALIBRATION_DOWN_TIME = 1

BLINKY_TIME = 0.15

# Tube states
TUBE_STATE_DEPLOY = 1
TUBE_STATE_RETRIEVE = 2
TUBE_STATE_OFF = 3

# key:   digital/output input port
# value: arm position
arm_height_map = {
    3: ARM_1,       #   2 
    6: ARM_2,       # 1
    2: ARM_3,       #   4
    5: ARM_4,       # 3
    1: ARM_5,       #   6
    4: ARM_6        # 5
}

class Arm(object):
    
    def __init__(self):
        '''Constructor'''
        self.vertical_motor = wpilib.CANJaguar(10)
        self.scooper_motor = wpilib.Relay(1)
        
        # configure the vertical motor to be in the correct mode and such
        self.vertical_motor.SetPositionReference( wpilib.CANJaguar.kPosRef_QuadEncoder )
        self.vertical_motor.ConfigEncoderCodesPerRev( ENCODER_TURNS_PER_REVOLUTION )
        #self.vertical_motor.ConfigSoftPositionLimits( ARM_POSITION_MIN, ARM_POSITION_MAX )
        
        self.manual_mode = False
        self.automatic_mode = False
        self.calibration_mode = True
        
        self.calibration_timer = None
        
        # For the blinkies
        self.timer = wpilib.Timer()
        self.timer.Start()
        self.blinky = True
        
        self.encoder_print = SometimesPrint()
        
        # Manual motor value of vertical motor
        self.vertical_motor_value = None
        
        # Position we want the arm to be in
        self.vertical_motor_position = None
        
        self.tube_state = TUBE_STATE_OFF
    
    def set_vertical_position(self, y):
        '''Tell the arm to go to a specific position'''
        self.vertical_motor_position = y
        self.automatic_mode = True
        
    def manual_vertical_control(self, y):
    
        '''Tell the arm to go up or down'''
        self.vertical_motor_value = y
        self.manual_mode = True
        
    def arm_is_in_position(self):
        '''Check if placement of arm is correct'''
        
        if self.vertical_motor_position is not None:
            # automated mode
            position = self.vertical_motor.GetPosition()
            
            if position <= self.vertical_motor_position + ARM_TOLERANCE and position >= self.vertical_motor_position - ARM_TOLERANCE:
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
        
        
    def set_jaguar_control_mode(self, mode):
        '''Sets the control mode for the Jaguar'''
    
        if self.vertical_motor.GetControlMode() != mode:
            self.vertical_motor.ChangeControlMode(mode)
            self.vertical_motor.EnableControl()
        
    def set_arm_indicators(self, ds):
        '''Controls the arm-related LED indicators on the driver station
        
        If the user selects a position, then this will make the indicated
        light turn on. If the arm is moving towards the position, it will
        blink rapidly. When it is at the position, then it is solid
        
        '''
    
        # use this array to set all lights off by default
        lights = [ False, False, False, False, False, False ]
        
        if self.vertical_motor_position is not None:
            
            if self.timer.HasPeriodPassed(BLINKY_TIME):
                self.blinky = not self.blinky
            
            # if the arm is in position, then the light stays solid. 
            # Otherwise, it just blinks
            on_off = self.blinky or self.arm_is_in_position()
            
            # Use the arm map to map buttons/lights to arm positions
            for k,v in arm_height_map.items():
                if self.vertical_motor_position == v:
                    lights[k-1] = on_off
                
        # actually turn the lights on or off
        i = 1
        for light in lights:
            
            # Note: Lights are inverted, so True turns it off
            ds.SetDigitalOut(i, not light)
            i += 1
        
    def do_control_loop(self):
        '''This control function makes sure all the motors are doing 
        what they should be doing. Works in autonomous and teleop modes'''
    
        # Telling the scooper what to do
        if self.tube_state == TUBE_STATE_DEPLOY:
            self.scooper_motor.Set(wpilib.Relay.kForward)
        elif self.tube_state == TUBE_STATE_RETRIEVE:
            self.scooper_motor.Set(wpilib.Relay.kReverse)
        elif self.tube_state == TUBE_STATE_OFF:
            self.scooper_motor.Set(wpilib.Relay.kOff)
        
        # Make sure to turn the scooper off each time so it doesn't run continuously
        self.tube_state = TUBE_STATE_OFF
        
        self.encoder_print.print( "Arm: C: %s encoder value: %f" % (self.calibration_mode, self.vertical_motor.GetPosition()) )
        
        
        # if we're in calibration mode, take any opportunity that we have
        # to detect that we've reached a calibration point
        if self.calibration_mode:
            
            # once we've reached a spot, then calibration mode is done
            if self.vertical_motor.GetReverseLimitOK():
                # we've reached the bottom
                self.calibration_mode = False
                
            elif self.vertical_motor.GetForwardLimitOK():
                # we've reached the top
                self.calibration_mode = False
        
        
        if self.manual_mode:
            # If it is in Manual mode, and...
            
            # If the Jaguar is not in the correct control mode, set the output mode
            self.set_jaguar_control_mode( wpilib.CANJaguar.kPercentVbus )
            
            # Set motor value and turn off Manual Mode and automatic mode
            self.vertical_motor.Set(self.vertical_motor_value)
            self.vertical_motor_value = None
            
            # reset our state
            self.manual_mode = False
            self.automatic_mode = False
            self.vertical_motor_position = None
            
        elif self.calibration_mode and self.automatic_mode:
            
            # Calibration mode is special -- only run this the first time that
            # we go into automatic mode
            
            if self.calibration_timer is None:
                self.calibration_timer = wpilib.Timer()
                self.calibration_timer.Start()
                
            if self.calibration_timer.HasPeriodPassed( CALIBRATION_DOWN_TIME ):
                # go back up
                self.vertical_motor.Set( 1.0 )
            else:
                # go down
                self.vertical_motor.Set( -1.0 )
                
        elif self.automatic_mode:
                
            # If the Jaguar is not in the correct control mode, set the output mode
            self.set_jaguar_control_mode( wpilib.CANJaguar.kPosition )
            
            # Set motor value               
            self.vertical_motor.Set(self.vertical_motor_position)
            
            # reset our state
            self.manual_mode = False
            self.automatic_mode = False
                    
        elif self.vertical_motor_position == None:
        
            # If the Jaguar was previously in manual mode, turn the motor off
            self.vertical_motor.Set(0)
                    
                    