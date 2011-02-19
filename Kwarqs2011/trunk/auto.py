
import wpilib

LT_STEER_AMOUNT = 0.65
LT_MAX_CROSS_DISTANCE = 24.0
LT_DEPLOY_RANGE = 24.0
LT_TOO_CLOSE = 48.0


class Auto(object):

    def __init__(self):
        '''Constructor'''
        
        # ultrasonic sensor
        self.ultrasonic = wpilib.Ultrasonic(13,14)
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
        
    
    def update_line_tracking(self, ds):
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
                
        # compute the single value from the 3 sensors. Notice that the bits
        # for the outside sensors are flipped depending on left or right
        # fork. Also the sign of the steering direction is different for left/right.
        if self.ltp_go_right:
            binaryValue = right * 4 + middle * 2 + left
        else:
            binaryValue = left * 4 + middle * 2 + right
                
        # default to no turn
        self.lt_steering = 0                                
        
        # just the outside sensor - drive straight
        if binaryValue == 1:
            self.lt_steering = 0                   
           
        # all sensors - maybe at the "T"
        elif binaryValue == 7:      
            
            if self.ultrasonic.GetRangeInches() > LT_MAX_CROSS_DISTANCE:
                self.lt_at_line = True
        
        # no sensors - apply previous correction
        elif binaryValue == 0:
            
            if self.ltp_previous == 0 or self.ltp_previous == 1:
                self.lt_steering = LT_STEER_AMOUNT
            else:
                self.lt_steering = -LT_STEER_AMOUNT
        
        # anything else, steer back to the line
        else:
            self.lt_steering = -LT_STEER_AMOUNT
                
        # useful debugging output for tuning your power profile and steering gain
        #if binaryValue != self.ltp_previous:
        #    print("Time: %2.2f sensor: %d speed: %1.2f turn: %1.2f atCross: %d\n" % (time, binaryValue, speed, turn, self.lt_at_line))
            
        if binaryValue != 0:
            self.ltp_previous = binaryValue
    
    
    def line_tracking_drive(self, drive):
    
        x = self.lt_steering

        # Relative distance from wall
        y = 0
    
        drive.ArcadeDrive(y, x, False)
    
        
    def do_control_loop(self, drive, arm):
        '''Implements automated tube placement'''
        
        # Is arm at height?
        if arm.arm_is_in_position() == True:
            
            # Yes! -> In deploy range?
            if self.ultrasonic.GetRangeInches() <= LT_DEPLOY_RANGE:
                
                # Is the middle sensor on?
                if self.middle_tracker.Get(1):
                
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
        elif self.ultrasonic.GetRangeInches() <= LT_TOO_CLOSE:
    
            # Yes! -> Stop motors
            drive.ArcadeDrive(0, 0, False)
        
        # No! -> Line tracking
        else:
            self.line_tracking_drive(drive)
            
