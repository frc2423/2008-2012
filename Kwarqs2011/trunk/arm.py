import wpilib
from util import *

# The vertical positions for the arm, relative to the bottom!
ARM_TOP = 8.00
ARM_1 = 8.00       #   2
ARM_2 = 8.00       # 1
ARM_3 = 6.26       #   4
ARM_4 = 6.10       # 3
ARM_5 = 3.53       #   6
ARM_6 = 2.88      # 5

# these are relative to the bottom!
arm_offsets = [ ARM_1, ARM_2, ARM_3, ARM_4, ARM_5, ARM_6 ]
arm_height = []

# PID constants
ARM_P = 0.95
ARM_I = 0.0
ARM_D = 0.0


ARM_TOLERANCE = .2
ENCODER_TURNS_PER_REVOLUTION = 360

CALIBRATION_DOWN_TIME = 1

BLINKY_TIME = 0.15

# Tube states
TUBE_STATE_DEPLOY = 1
TUBE_STATE_RETRIEVE = 2
TUBE_STATE_OFF = 3

# key:   digital/output input port
# value: arm position number
arm_position_map = {
    3: 0,       #   2 
    6: 1,       # 1
    2: 2,       #   4
    5: 3,       # 3
    1: 4,       #   6
    4: 5        # 5
}

class Arm(object):
    
    def __init__(self):
        '''Constructor'''
        self.vertical_motor = wpilib.CANJaguar(10)
        self.scooper_motor = wpilib.Relay(1)
        
        # configure the vertical motor to be in the correct mode and such
        self.vertical_motor.ConfigEncoderCodesPerRev( ENCODER_TURNS_PER_REVOLUTION )
        self.vertical_motor.ConfigNeutralMode( wpilib.CANJaguar.kNeutralMode_Brake )
        
        self.manual_set = False
        self.position_set = False
        self.hold_set = False
        self.calibration_mode = True
        
        self.calibration_timer = None
        
        # For the blinkies
        self.timer = wpilib.Timer()
        self.timer.Start()
        self.blinky = True
        
        self.encoder_print = PrintTimer()
        
        # Manual motor value of vertical motor
        self.vertical_motor_value = None
        self.vertical_motor_hold = None
        
        # Position (1-6) we want the arm to be in
        self.position_set_value = None
        
        self.hold_position = None
        
        self.tube_state = TUBE_STATE_OFF
    
    def set_vertical_position(self, y):
        '''Tell the arm to go to a specific position'''
        self.position_set_value = y
        self.position_set = True
        
    def manual_vertical_control(self, y):
    
        '''Tell the arm to go up or down'''
        self.manual_value = y
        self.manual_set = True
        
    def set_hold_position(self):
        '''Tells the arm to stay exactly where it is'''
        self.hold_position = self.vertical_motor.GetPosition()
        self.hold_set = True
        
    def arm_is_in_position(self):
        '''Check if placement of arm is correct'''
        
        if self.hold_position is not None:
            # automated mode
            position = self.vertical_motor.GetPosition()
            
            if position <= self.hold_position + ARM_TOLERANCE and position >= self.hold_position - ARM_TOLERANCE:
                return True
            else:
                return False
                
        else:
            # We're in manual mode here -- we assume that if the human
            # has let go of the trigger ( manual_vertical_control hasn't
            # been called ), then we must be in the correct position
            return self.manual_value == None
    
        
    def deploy_tube(self):
        '''Place the tube on the peg'''
        self.tube_state = TUBE_STATE_DEPLOY
        
    def retrieve_tube(self):
        '''Pick up the tube'''
        self.tube_state = TUBE_STATE_RETRIEVE
        
        
    def set_jaguar_control_mode(self, mode):
        '''Sets the control mode for the Jaguar'''
    
        if self.vertical_motor.GetControlMode() != mode:
        
            print( "Trying to change the mode to %d" % int(mode) )
        
            self.vertical_motor.ChangeControlMode(mode)
        
            if mode == wpilib.CANJaguar.kPosition:
                self.vertical_motor.SetPositionReference( wpilib.CANJaguar.kPosRef_QuadEncoder )
                self.vertical_motor.SetPID( ARM_P, ARM_I, ARM_D )
        
            self.vertical_motor.EnableControl()
        
    def set_arm_indicators(self, ds):
        '''Controls the arm-related LED indicators on the driver station
        
        If the user selects a position, then this will make the indicated
        light turn on. If the arm is moving towards the position, it will
        blink rapidly. When it is at the position, then it is solid
        
        '''
    
        # use this array to set all lights off by default
        lights = [ False, False, False, False, False, False ]
        
        if self.position_set_value is not None:
            
            if self.timer.HasPeriodPassed(BLINKY_TIME):
                self.blinky = not self.blinky
            
            # if the arm is in position, then the light stays solid. 
            # Otherwise, it just blinks
            on_off = self.blinky or self.arm_is_in_position()
            
            # Use the arm map to map buttons/lights to arm positions
            for k,v in arm_position_map.items():
                if self.position_set_value == v:
                    lights[k-1] = on_off
                
        # actually turn the lights on or off
        i = 1
        for light in lights:
            
            # Note: Lights are inverted, so True turns it off
            ds.SetDigitalOut(i, not light)
            i += 1
            
    def _reset_state(self, reset_manual=True, reset_automatic=True):
        
        self.manual_set         = False
        self.position_set       = False
        self.hold_set           = False
            
        if reset_manual:
            self.manual_value       = None
            
        if reset_automatic:
            self.position_set_value = None
            
    def _do_calibration(self):
        '''This routine only gets called when we're doing calibration'''
        
        bottom_position = None
        
        # once we've reached a spot, then calibration mode is done
        if not self.vertical_motor.GetReverseLimitOK():
            # we've reached the bottom
            self.calibration_mode = False
            
            # this is the bottom position... 
            bottom_position = self.vertical_motor.GetPosition()
            
        elif not self.vertical_motor.GetForwardLimitOK():
            # we've reached the top
            self.calibration_mode = False
            
            bottom_position = self.vertical_motor.GetPosition() - ARM_TOP
            
        if bottom_position is not None:
            
            # if we found a calibration point, then create the arm height
            # position table based on our encoder positions
            
            for i in arm_offsets:
                arm_height.append( bottom_position + i )
        
        
    def do_control_loop(self):
        '''This control function makes sure all the motors are doing 
        what they should be doing. Works in autonomous and teleop modes'''
    
        # Telling the scooper what to do
        if self.tube_state == TUBE_STATE_DEPLOY:
            self.scooper_motor.Set(wpilib.Relay.kReverse)
        elif self.tube_state == TUBE_STATE_RETRIEVE:
            self.scooper_motor.Set(wpilib.Relay.kForward)
        elif self.tube_state == TUBE_STATE_OFF:
            self.scooper_motor.Set(wpilib.Relay.kOff)
        
        # Make sure to turn the scooper off each time so it doesn't run continuously
        self.tube_state = TUBE_STATE_OFF
        
        if self.encoder_print.should_print():
            print( "Arm: C: %s; mode: %d; encoder value: %f" % (self.calibration_mode, int(self.vertical_motor.GetControlMode()), self.vertical_motor.GetPosition()) )
        
        
        # if we're in calibration mode, take any opportunity that we have
        # to detect that we've reached a calibration point
        if self.calibration_mode:
            self._do_calibration()
            
        
        # this set of if statements determines what to set the motor to
        motor_value = 0
        
        if self.hold_set:
        
            print("In hold state")
        
            # Set the correct output mode
            self.set_jaguar_control_mode( wpilib.CANJaguar.kPosition )
        
            # reset our state
            self._reset_state()
        
        elif self.manual_set:
            
            # If the Jaguar is not in the correct control mode, set the output mode
            self.set_jaguar_control_mode( wpilib.CANJaguar.kPercentVbus )
            
            # Set motor value and turn off Manual Mode and automatic mode
            motor_value = self.manual_value
            
            # reset our state
            self._reset_state(reset_manual=False)
            
            
        elif self.calibration_mode and self.position_set:
            
            # Calibration mode is special -- only run this the first time that
            # we try to set the position automatically
            
            if self.calibration_timer is None:
                self.calibration_timer = wpilib.Timer()
                self.calibration_timer.Start()
                
            # enforce the motor mode
            self.set_jaguar_control_mode( wpilib.CANJaguar.kPercentVbus )
                
            if self.calibration_timer.Get() >= CALIBRATION_DOWN_TIME:
                # go back up
                motor_value = 1.0
            else:
                # go down
                motor_value = -1.0
                
        elif self.position_set:
        
            print("In position set")
                
            # Set the correct output mode
            self.set_jaguar_control_mode( wpilib.CANJaguar.kPosition )
            
            # Set the position to hold at        
            self.hold_position = arm_height[ self.position_set_value ]
            
            # reset our state
            self._reset_state(reset_automatic=False)
            
            
        # ok, now that we've figured out what motor value to set
        # set the motor value to the value
        
        if self.hold_position is not None:
        
            # set the motor to the hold position
            self.vertical_motor.Set( self.hold_position )
            
        else:
        
            # If the Jaguar was previously in manual mode, turn the motor off
            self.vertical_motor.Set( motor_value )
                    
                    