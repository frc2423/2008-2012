
import wpilib
from util import *

# autonomous mode stuff
LTA_STEER_LEFT = -0.1
LTA_STEER_MIDDLE = 0.0
LTA_STEER_RIGHT = 0.1

# teleoperated mode parameters
LTT_STEER_LEFT = -0.5
LTT_STEER_MIDDLE = 0.3
LTT_STEER_RIGHT = 0.5


LT_MAX_CROSS_DISTANCE = 24.0
LT_DEPLOY_RANGE = 36.0
LT_TOO_CLOSE = 72.0


class Auto(object):

    def __init__(self):
        '''Constructor'''
        
        # ultrasonic sensor
        self.ultrasonic = wpilib.Ultrasonic(5,4)
        wpilib.Ultrasonic.SetAutomaticMode( True )
        
        # line tracking sensors
        self.left_tracker = wpilib.DigitalInput(1)  
        self.middle_tracker = wpilib.DigitalInput(2)
        self.right_tracker = wpilib.DigitalInput(3)
        
        # line tracking state variables
        self.ltp_previous = 0
        self.ltp_go_right = True
       
        # outputs of line tracking routine
        self.lt_at_line = False     # Set to True if we're at the end of the line
        self.lt_steering = 0        # value to steer left or right
        
        self.line_timer = PrintTimer()
        
        
    
    def update_line_tracking(self, ds, autonomous_time):
        '''
        Decides to go left or right based on the state of line tracking
        sensors. This function does not actually cause the motors to do
        anything, it just reports the state of the line tracking sensors.
        The output of this function is the following variables:
        
            self.lt_at_line     Set to true if it thinks we're at the end
            self.lt_steering    Motor curve value to go left or right
        
        '''
        
        # first, activate the line tracking LED when we're on the middle line
        ds.SetDigitalOut( 7, not self.middle_tracker.Get() )
        
        # read the line tracking sensors
        if self.left_tracker.Get():        
            left = 1
        else:
            left = 0
        
        if self.middle_tracker.Get():
            middle = 1
        else:
            middle = 0
            
        if self.right_tracker.Get():
            right = 1
        else:
            right = 0
            
        
        if left:
            if autonomous_time is not None:
                self.lt_steering = LTA_STEER_LEFT
            else:
                self.lt_steering = LTT_STEER_LEFT
            
        if middle:
            if autonomous_time is not None:
                self.lt_steering = LTA_STEER_MIDDLE
            else:
                self.lt_steering = LTT_STEER_MIDDLE
            
        if right: 
            if autonomous_time is not None:
                self.lt_steering = LTA_STEER_RIGHT
            else:
                self.lt_steering = LTT_STEER_RIGHT
        
        #if left and middle and right:
        
        #    self.lt_steering = 0
        
            #if self.ultrasonic.IsRangeValid() and self.ultrasonic.GetRangeInches() > LT_MAX_CROSS_DISTANCE:
            #    self.lt_at_line = True
            
            
        # compute the single value from the 3 sensors. Notice that the bits
        # for the outside sensors are flipped depending on left or right
        # fork. Also the sign of the steering direction is different for left/right.
        #if self.ltp_go_right:
        #    binaryValue = right * 4 + middle * 2 + left
        #else:
        #    binaryValue = left * 4 + middle * 2 + right
        
        # just the outside sensor - drive straight
        # if binaryValue == 1:
            # self.lt_steering = 0                   
           
        # # all sensors - maybe at the "T"
        # elif binaryValue == 7:      
            
            # if self.ultrasonic.IsRangeValid() and self.ultrasonic.GetRangeInches() > LT_MAX_CROSS_DISTANCE:
                # self.lt_at_line = True
        
        # # no sensors - apply previous correction
        # elif binaryValue == 0:
            
            # if self.ltp_previous == 0 or self.ltp_previous == 1:
                # self.lt_steering = LT_STEER_AMOUNT
            # else:
                # self.lt_steering = -LT_STEER_AMOUNT
        
        # # anything else, steer back to the line
        # else:
            # self.lt_steering = -LT_STEER_AMOUNT
                
        # useful debugging output for tuning your power profile and steering gain
        #if binaryValue != self.ltp_previous:
        #    print("Time: %2.2f sensor: %d speed: %1.2f turn: %1.2f atCross: %d\n" % (time, binaryValue, speed, turn, self.lt_at_line))
            
        #if binaryValue != 0:
        #   self.ltp_previous = binaryValue
            
        if self.line_timer.should_print():
            print( "Line tracking: L: %s; M: %s; R: %s; Speed: %f; Wall: %s" % (left, middle, right, self.lt_steering, self.ultrasonic.GetRangeInches() ) )
    
    
    def line_tracking_drive(self, drive):
    
        x = self.lt_steering

        # Relative distance from wall
        distance = 0
        if self.ultrasonic.IsRangeValid():
            distance = self.ultrasonic.GetRangeInches()
        
        if distance > LT_DEPLOY_RANGE + 36.0:
            y = -0.6
        elif distance > LT_DEPLOY_RANGE + 24.0:
            y = -0.5
        elif distance > LT_DEPLOY_RANGE + 12.0:
            y = -0.3
        elif distance > LT_DEPLOY_RANGE:
            y = -0.2
        else:
            y = 0.0
    
        drive.ArcadeDrive(y, x, False)
    
        
    def do_control_loop(self, drive, arm, autonomous_time):
        '''Implements automated tube placement'''
        
        #arm_in_position = arm.arm_is_in_position()
        arm_in_position = True
        
        # autonomous mode: get out of dodge if time expires!
        if autonomous_time > 12.0
            arm.deploy_tube()
            drive.ArcadeDrive( 0, 0.3, False )
            return
        
        # Is arm at height?
        if True:
            
            # Yes! -> In deploy range?
            if self.ultrasonic.IsRangeValid() and self.ultrasonic.GetRangeInches() <= LT_DEPLOY_RANGE:
                
                # Is the middle sensor on?
                if autonomous_time is not None or self.middle_tracker.Get():
                
                    # Turn drive motors off
                    drive.ArcadeDrive(0, 0, False)
                
                    # Yes! -> Deploy tube
                    arm.deploy_tube()
                    
                # No! -> Line tracking
                else:
                    self.line_tracking_drive(drive)
                    
            # No! -> Set speed, then...
            else:
                # Line tracking
                self.line_tracking_drive(drive)
        
        # No! -> Are we too close?
        elif self.ultrasonic.IsRangeValid() and self.ultrasonic.GetRangeInches() <= LT_TOO_CLOSE:
    
            # Yes! -> Stop motors
            drive.ArcadeDrive(0, 0, False)
        
        # No! -> Line tracking
        else:
            self.line_tracking_drive(drive)
            
