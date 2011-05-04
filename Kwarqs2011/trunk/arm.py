'''
    Routines to make the arm work
    
    TODO: The state machine for this is a bit unwieldy at this point,
    needs some massaging to make it more managable.
    
    The idea is that there are two phases of each robot control period: 
    a 'decision' phase, and a 'control' phase. The decision phase 
    decides where things should go, and then the control phase actually
    sends the decisions down to the motors and stuff like that. This kind
    of thing makes our arm easy to control during autonomous mode
    
    Philosophy bit:
    
    For the control phase, each time something gets sent to the motors,
    then the initial input is reset to None in order to provide for a 
    different input if
    
    However, for constant inputs (ie, go to a position and hold it), a
    different strategy must be adopted. 
    
'''

try:
    import wpilib
except:
    import fake_wpilib as wpilib

from util import *

# the expected distance between the top and bottom limit switches
ARM_TOP = 18.4

# The vertical positions for the arm, relative to the bottom. In percentages!
#ARM_1 = 19.0        #   2  -- 0
#ARM_2 = 19.0        # 1    -- 1
#ARM_3 = 17.1        #   4  -- 2
#ARM_4 = 19.0        # 3    -- 3
#ARM_5 = 9.0         #   6  -- 4
#ARM_6 = 11.2        # 5    -- 5

ARM_1 = 2.00
ARM_2 = 2.00
ARM_3 = 0.93
ARM_4 = 2.00
ARM_5 = 0.49
ARM_6 = 0.61

ARM_TOPTOP = 1000.0
ARM_BOTTOM = -1000.0

'''
    bottom = .1722, .204, .22, .151, .206
    first = 9.08, 9.31
    next = 17.30, 17.02  
    top = 18.63, 18.60

    bottom = .26, .21, .14
    first = 11.41
    next = 18.62
    top = 18.62
'''





# these are relative to the bottom!
arm_offsets = [ ARM_1, ARM_2, ARM_3, ARM_4, ARM_5, ARM_6, ARM_TOPTOP, ARM_BOTTOM ]
arm_height = []
actual_top_position = None
actual_bottom_position = None



# PID constants
ARM_P = 1500.0
ARM_I = 0.0
ARM_D = 0.0

ARM_UP = 1.0
ARM_DOWN = -1.0

ARM_TOLERANCE = .2
ENCODER_TURNS_PER_REVOLUTION = 360

CALIBRATION_DOWN_TIME = 1

BLINKY_TIME = 0.15

# the thump positions for the arm, matches the ARM_X positions 
# as shown above
THUMP_1 = .7
THUMP_2 = .7
THUMP_3 = .35
THUMP_4 = .35
THUMP_5 = .35
THUMP_6 = .35 

# Thump motor PID constants
THUMP_P = 1500.0
THUMP_I = 0.0
THUMP_D = 0.0

THUMP_DOWN = -1.0

THUMP_MIN_POSITION = .40
THUMP_MAX_POSITION = .65

thumps = [ THUMP_1, THUMP_2, THUMP_3, THUMP_4, THUMP_5, THUMP_6 ]

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
    4: 5       # 5
}

class Arm(object):

    # button definitions
    TOP = 6
    BOTTOM = 7

    def __init__(self):
        '''Constructor'''
        
        self.scooper_motor = wpilib.Relay(1)
        
        # configure the thump motor 
        self.thump_motor = wpilib.CANJaguar(11)
        self.thump_motor.ConfigNeutralMode( wpilib.CANJaguar.kNeutralMode_Brake )
        self.thump_motor.SetPositionReference( wpilib.CANJaguar.kPosRef_Potentiometer )
        self.thump_motor.ConfigPotentiometerTurns( 1 )
        self.thump_motor.current_mode = wpilib.CANJaguar.kPercentVbus
        
        # configure the vertical motor
        self.vertical_motor = wpilib.CANJaguar(10)
        self.vertical_motor.SetPositionReference( wpilib.CANJaguar.kPosRef_QuadEncoder )
        self.vertical_motor.ConfigEncoderCodesPerRev( ENCODER_TURNS_PER_REVOLUTION )
        self.vertical_motor.ConfigNeutralMode( wpilib.CANJaguar.kNeutralMode_Brake )
        self.vertical_motor.current_mode = wpilib.CANJaguar.kPercentVbus
        
        # state variables
        self.manual_set = False
        self.position_set = False
        self.hold_set = False
        self.calibration_mode = True
        
        self.thump_value_set = False
        self.thump_position_set = False
        
        self.calibration_timer = None
        
        # For the blinkies
        self.blinky_timer = PrintTimer( BLINKY_TIME )
        self.blinky = True
        
        self.print_timer = PrintTimer()
        
        # Manual motor value of vertical motor
        self.vertical_motor_value = None
        self.vertical_motor_hold = None
        
        # Position (1-6) we want the arm to be in
        self.position_set_value = None
        
        # The last manual value that was set
        self.manual_value = None
        
        # If not none, a position that we should hold the arm at vertically
        self.hold_position = None
        
        # Position (0-1.0) we want the arm to be angled
        self.thump_position = None
        self.thump_value = None
        
        self.thump_drop_timer = None
        
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
        
    def set_thump_position(self, y):
        '''Sets the position of the thump motor: 0 is all the way down, 1 is all the way up'''
        #if y > 1.0:
        #    self.thump_position = 1.0
        #elif y < 0.0:
        #    self.thump_position = 0.0
        #else:
        #    self.thump_position = y
            
        #self.thump_position_set = True
        
        # We removed the pot, so let's make sure we don't accidentally try to switch modes
        pass
        
    def do_thump_drop(self, seconds):
        '''Drop the thump motor for N seconds'''
        if self.thump_drop_timer is not None:
            return
            
        self.thump_drop_timer = wpilib.Timer()
        self.thump_drop_timer.Start()
        self.thump_drop_timer.drop_time = seconds
        print("[arm] Thump drop time set to %s" % str(seconds))
            
            
            
    def manual_thump_control(self, y):
        '''Tell the thump motor to go up or down'''
        self.thump_value = y
        self.thump_value_set = True
        
    def at_top_limit(self):
        '''Returns true if we're hitting the top of the elevator'''
        return not self.vertical_motor.GetForwardLimitOK()
        
    def at_bottom_limit(self):
        '''Returns true if we're hitting the bottom of the elevator'''
        return not self.vertical_motor.GetReverseLimitOK() 
        
    def arm_is_in_position(self):
        '''Check if placement of arm is correct'''
        
        if self.hold_position is not None:
            # automated mode
            position = self.vertical_motor.GetPosition()
            
            if position <= self.hold_position + ARM_TOLERANCE and position >= self.hold_position - ARM_TOLERANCE:
                return True
            else:
                return False
                
        elif self.calibration_mode and self.position_set:
        
            # If we're calibrating, we can't possibly be ready yet
            return False
                
        elif self.manual_value == None:
            
            # We're in manual mode here -- we assume that if the human
            # has let go of the trigger ( manual_vertical_control hasn't
            # been called ), then we must be in the correct position
        
            return True
            
        return False
    
        
    def deploy_tube(self):
        '''Place the tube on the peg'''
        self.tube_state = TUBE_STATE_DEPLOY
        
    def retrieve_tube(self):
        '''Pick up the tube'''
        self.tube_state = TUBE_STATE_RETRIEVE
        
        
    def _set_vertical_control_mode(self, mode):
        '''Sets the control mode for the Jaguar'''
    
        # current_mode is a custom variable we attach to the thing to
        # prevent us from querying the thing over CAN each time we want
        # to do a manual movement (which is what happens when you call
        # GetControlMode() ... )
        if self.vertical_motor.current_mode != mode:
        
            self.vertical_motor.ChangeControlMode(mode)
            self.vertical_motor.current_mode = mode
        
            if mode == wpilib.CANJaguar.kPosition:
                
                self.vertical_motor.ConfigEncoderCodesPerRev( ENCODER_TURNS_PER_REVOLUTION )
                self.vertical_motor.SetPositionReference( wpilib.CANJaguar.kPosRef_QuadEncoder )
                self.vertical_motor.SetPID( ARM_P, ARM_I, ARM_D )
                self.vertical_motor.EnableControl( self.vertical_motor.GetPosition() )
                
            else:
                self.vertical_motor.DisableControl()
    
                
    def _set_thump_control_mode(self, mode):
        '''Sets the control mode for the thump motor jaguar'''
        
        # current_mode is a custom variable we attach to the thing to
        # prevent us from querying the thing over CAN each time we want
        # to do a manual movement (which is what happens when you call
        # GetControlMode() ... )
        if self.thump_motor.current_mode != mode:
        
            self.thump_motor.ChangeControlMode(mode)
            self.thump_motor.current_mode = mode
        
            if mode == wpilib.CANJaguar.kPosition:
        
                self.thump_motor.SetPositionReference( wpilib.CANJaguar.kPosRef_Potentiometer )
                self.thump_motor.ConfigPotentiometerTurns( 1 )
                #self.thump_motor.ConfigSoftPositionLimits( THUMP_MIN_POSITION, THUMP_MAX_POSITION )
                self.thump_motor.SetPID( THUMP_P, THUMP_I, THUMP_D )
        
                self.thump_motor.EnableControl()
            else:
                self.thump_motor.DisableControl()
    
        
    def set_arm_indicators(self, ds):
        '''
        Controls the arm-related LED indicators on the driver station
        
        If the user selects a position, then this will make the indicated
        light turn on. If the arm is moving towards the position, it will
        blink rapidly. When it is at the position, then it is solid.
        '''
    
        # use this array to set all lights off by default
        lights = [ False, False, False, False, False, False ]
        
        if self.position_set_value is not None:
            
            if self.blinky_timer.should_print():
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
            
    
                
        
    
    #############################################
    # Scooper motor control functions
    #############################################    
    
    def _do_scooper_control(self):
        '''Control loop for the scooper motor'''
    
         # Telling the scooper what to do
        if self.tube_state == TUBE_STATE_DEPLOY:
            self.scooper_motor.Set(wpilib.Relay.kForward)
        elif self.tube_state == TUBE_STATE_RETRIEVE:
            self.scooper_motor.Set(wpilib.Relay.kReverse)
        elif self.tube_state == TUBE_STATE_OFF:
            self.scooper_motor.Set(wpilib.Relay.kOff)
        
        # Make sure to turn the scooper off each time so it doesn't run continuously
        self.tube_state = TUBE_STATE_OFF
        
        
    #############################################
    # Thump motor control functions
    #############################################
        
    def _translate_thump(self, z):
    
        # P = Xmax - (Ymax - Y)( (Xmax - Xmin) / (Ymax - Ymin) )
        value = THUMP_MAX_POSITION - ((1 - z)*( (THUMP_MAX_POSITION - THUMP_MIN_POSITION) / (1.0-0) ) )
        
        if value > THUMP_MAX_POSITION:
            return THUMP_MAX_POSITION
        elif value < THUMP_MIN_POSITION:
            return THUMP_MIN_POSITION
        return value
        
    def _invert_thump(self, p):
        '''Does the inverse of _translate_thump -- take a thump position and
        translate it into a value. Used only for diagnostics'''
        
        # -Y = (P - Xmax) / ((Xmax - Xmin) / (Ymax - Ymin))) + Ymax
        return ( (p - THUMP_MAX_POSITION) / ((THUMP_MAX_POSITION-THUMP_MIN_POSITION)/(1.0-0)) + 1.0)
        
    def _do_thump_control(self):
        '''Control loop for the thump motor'''
    
        # Ok, do the thump motor stuff. Same pattern
        thump_motor_value = 0
        
        # if we're dropping the thump motor... 
        if self.thump_drop_timer is not None:
        
            tm = self.thump_drop_timer.Get()
            
            if tm > self.thump_drop_timer.drop_time:
                print("[arm] Thump drop stopped after %s seconds (wanted %s seconds)" % (str(tm), str(self.thump_drop_timer.drop_time)))
                
                # delete the timer
                self.thump_drop_timer = None
            else:
                thump_motor_value = THUMP_DOWN
        
        elif self.thump_value_set:
        
            self._set_thump_control_mode( wpilib.CANJaguar.kPercentVbus )
        
            thump_motor_value = self.thump_value
        
            self.thump_value_set = False
            
            self.thump_position = None
            self.thump_position_set = False
        
        elif self.thump_position_set:

            if self.print_timer.should_print(10):
                print("[arm] WARNING: Thump motor in position mode: %s" % str(self.thump_position))
        
            # switch to positioning mode
            self._set_thump_control_mode( wpilib.CANJaguar.kPosition )
            
            self.thump_value_set = False
            self.thump_position_set = False
        
            
        # thump motor control
        if self.thump_position is not None:
            thump_motor_value = self._translate_thump( self.thump_position )
        
        self.thump_motor.Set( thump_motor_value )
        
        
    #############################################
    # Vertical motor control functions
    #############################################
    
    def _reset_vertical_state(self, reset_manual=True, reset_automatic=True):
        
        self.manual_set         = False
        self.position_set       = False
        self.hold_set           = False
            
        if reset_manual:
            self.manual_value = None
            
        if reset_automatic:
            self.position_set_value = None
    
    def _detect_calibration(self):
        '''This routine only gets called when we're doing calibration'''
        
        global actual_bottom_position
        global actual_top_position
        
        # the calibration point is the bottom edge of the elevator
        # the calibration range is the difference between the top and bottom
        
        # since the arm_offset items are expressed as percentage of the 
        # range, we use the range and the offset to calculate where the
        # arm height value should be relative to the entire range of
        # heights possible. This lets us automatically adjust the arm
        # positions in case the pulley system changes, or the encoder gets
        # screwed up.. not that this would ever happen :o
        
        calibration_point = None
        calibration_range = ARM_TOP
        
        # once we've reached a spot, then calibration mode is done
        if actual_bottom_position is None and self.at_bottom_limit():
        
            # we've reached a calibration point
            self.calibration_mode = False
            
            # this is the bottom position... 
            actual_bottom_position = self.vertical_motor.GetPosition()
            calibration_point = actual_bottom_position
            
            if actual_top_position is not None:
                calibration_range = actual_top_position - actual_bottom_position
            
            print("[arm] Arm calibration complete -- found bottom limit switch at %f" % actual_bottom_position);
            
            
        elif actual_top_position is None and self.at_top_limit():
        
            # we've reached a calibration point
            self.calibration_mode = False
            
            actual_top_position = self.vertical_motor.GetPosition()
            
            if actual_bottom_position is not None:
                calibration_range = actual_top_position - actual_bottom_position
                calibration_point = actual_bottom_position
            else:
                calibration_point = actual_top_position - ARM_TOP
            
            print("[arm] Arm calibration complete -- found top limit switch at %f" % actual_top_position);
            
            
        if calibration_point is not None:
            
            # if we found a calibration point, then create the arm height
            # position table based on our encoder positions
            del arm_height[:]
            for i in arm_offsets:
                arm_height.append( calibration_point + (i*calibration_range) )
        
    def _do_vertical_control(self):
        '''Control loop code for the vertical motor'''
    
        global actual_bottom_position
        global actual_top_position
    
        # if we haven't seen the top or bottom limit switches yet, take 
        # any opportunity that we have to detect that we've reached a 
        # calibration point
        if actual_bottom_position is None or actual_top_position is None:
            self._detect_calibration()
            
        
        # this set of if statements determines what to set the motor to
        motor_value = 0
        
        if self.hold_set:
        
            # Set the correct output mode
            self._set_vertical_control_mode( wpilib.CANJaguar.kPosition )
        
            self.hold_position = self.vertical_motor.GetPosition()
        
            if self.print_timer.should_print(1):
                print("[arm] In hold state: holding at %s" % str(self.hold_position))
        
            # reset our state
            self._reset_vertical_state()
        
        elif self.manual_set:
            
            # If the Jaguar is not in the correct control mode, set the output mode
            self._set_vertical_control_mode( wpilib.CANJaguar.kPercentVbus )
            
            # Set motor value and turn off Manual Mode and automatic mode
            motor_value = self.manual_value
            
            # reset our state
            self._reset_vertical_state(reset_manual=False)
            self.hold_position = None
            
            
        elif self.calibration_mode and self.position_set:
            
            # Calibration mode is special -- only run this the first time that
            # we try to set the position automatically
            
            # What we do: Run the arm downwards for a period of time
            # until we either reach the bottom or the period expires. Then
            # we race to the top if we haven't reached the bottom
            
            if self.calibration_timer is None:
                self.calibration_timer = wpilib.Timer()
                self.calibration_timer.Start()
                print("[arm] Arm calibration started")
                
            # enforce the motor mode
            self._set_vertical_control_mode( wpilib.CANJaguar.kPercentVbus )
                
            if self.calibration_timer.Get() >= CALIBRATION_DOWN_TIME or actual_bottom_position is not None:
                # go back up
                motor_value = ARM_UP
            else:
                # go down
                motor_value = ARM_DOWN
                
            self.hold_position = None
                
        elif self.position_set:
            
            if self.print_timer.should_print(2):
                try:
                    print("[arm] Position set to %s (height %s)" % (str(self.position_set_value), str(arm_height[self.position_set_value]) ))
                except:
                    print( arm_height, self.position_set_value )
                    exit(1)
                
            # Set the correct output mode
            self._set_vertical_control_mode( wpilib.CANJaguar.kPosition )
            
            # Set the position to hold at        
            self.hold_position = arm_height[ self.position_set_value ]
            
            # reset our state
            self._reset_vertical_state(reset_automatic=False)
            
            
            
        # ok, now that we've figured out what motor value to set
        # ... set the motor value to the value
        
        if self.hold_position is not None:
            
            if self.position_set_value is not None:
            
                # special case: if we keep bumping the limit switches when we're in
                # a position mode, just reset that position to the spot we're 
                # currently in so that arm_is_in_position will return true
            
                pos = self.vertical_motor.GetPosition()
                
                if pos < arm_height[ self.position_set_value ] and self.at_top_limit():
                    self.hold_position = pos
                    arm_height[ self.position_set_value ] = pos
                elif pos > arm_height[ self.position_set_value ] and self.at_bottom_limit():
                    self.hold_position = pos
                    arm_height[ self.position_set_value ] = pos
            
            # set the motor to the hold position
            motor_value = self.hold_position
            

        # Always set a motor value here. Either 0 after manual mode
        # exits, or whatever the current hold/manual value is
        self.vertical_motor.Set( motor_value )
        
        
    def do_control_loop(self):
        '''This control function makes sure all the motors are doing 
        what they should be doing. Works in autonomous and teleop modes'''
    
        self._do_scooper_control()
        
        self._do_thump_control()
        
        self._do_vertical_control()
                    
                    
    def print_diagnostics(self):
        '''These are useful to use to figure out if the arm is working correctly'''
    
        print( "*** ARM DIAGS ***" )
    
        print( "Vertical  : Mode: %d; encoder value: %f; Set: %f; In position: %s" % ( \
                int(self.vertical_motor.GetControlMode()), 
                self.vertical_motor.GetPosition(),
                self.vertical_motor.Get(),
                str(self.arm_is_in_position()) ))
                                
        print( "Modes     : cal_mode: %s(%s-%s); pos_mode: %s(%s); man_mode: %s(%s); hold: %s(%s)" % (\
                str(self.calibration_mode),
                str( actual_bottom_position ),
                str( actual_top_position ),
                str(self.position_set),
                str(self.position_set_value),
                str(self.manual_set),
                str(self.manual_value),
                str(self.hold_set),
                str(self.hold_position) ))
            
        #t_p = self.thump_motor.GetPosition()
        #    
        #print( 'Thump    : Position: %f; Set: %f; Input: %s; ValueForPos: %s' % ( \
        #        t_p,
        #        self.thump_motor.Get(),
        #        str(self.thump_position),
        #        str(self._invert_thump(t_p))))
        
        print( 'Offsets(%%): %s' % ' '.join( [ str(i) for i in arm_offsets] ) )
        print( 'Heights   : %s' % ' '.join( [ str(i) for i in arm_height] ) )
                    
        
        