
try:
    import wpilib
except:
    import fake_wpilib as wpilib
    
from util import *

# autonomous mode stuff
LTA_STEER_LEFT = -0.1
LTA_STEER_MIDDLE = 0.0
LTA_STEER_RIGHT = 0.1

# teleoperated mode parameters (no longer used)
LTT_STEER_LEFT = -0.5
LTT_STEER_MIDDLE = 0.3
LTT_STEER_RIGHT = 0.5

# autonomous mode distances:

LT_DEPLOY_RANGE = 36.0      # distance at which arm should deploy
LT_TOO_CLOSE = 96.0         # distance that robot should stop at if arm isn't right
LT_MAX = 240.0              # placeholder

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
        
        self.timer = PrintTimer()
        
        
    
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
            
        #if self.timer.should_print(1):
        #    print( "Line tracking: L: %s; M: %s; R: %s; Speed: %f; Wall: %s" % (left, middle, right, self.lt_steering, self.ultrasonic.GetRangeInches() ) )
    
    
    def line_tracking_drive(self, drive):
    
        x = self.lt_steering

        # Relative distance from wall
        wall_distance = 0
        if self.ultrasonic.IsRangeValid():
            wall_distance = self.ultrasonic.GetRangeInches()
        
        if wall_distance > LT_DEPLOY_RANGE + 40.0:
            y = -0.6
        elif wall_distance > LT_DEPLOY_RANGE + 32.0:
            y = -0.5
        elif wall_distance > LT_DEPLOY_RANGE + 16.0:
            y = -0.3
        elif wall_distance > LT_DEPLOY_RANGE:
            y = -0.2
        else:
            y = 0.0
    
        drive.ArcadeDrive(y, x, False)
    
        
    def do_control_loop(self, drive, arm, autonomous_time):
        '''Implements automated tube placement'''
        
        arm_in_position = arm.arm_is_in_position()
        wall_distance = LT_MAX
        
        if self.ultrasonic.IsRangeValid():
            wall_distance = self.ultrasonic.GetRangeInches()
        
        # autonomous mode: get out of dodge if time expires!
        if autonomous_time is not None and autonomous_time > 11.0:
        
            if autonomous_time < 16:
            
                if self.timer.should_print(2):
                    print("[auton: %4s; Arm: %5s; Wall: %6s] Getting out of dodge!!" % (\
                        str(autonomous_time),
                        str(arm_in_position),
                        str(wall_distance)
                    ))
            
                arm.manual_thump_control( 0.0 )
                arm.deploy_tube()
                
                # after it backs up, bring the arm back down 
                # so the operator can use it when the match starts
                if autonomous_time > 12.5:
                    arm.set_vertical_position( 5 )
                
                drive.ArcadeDrive( 0.35, 0, False )
            else:
            
                if self.timer.should_print(3):
                    print("[auton: %4s; Arm: %5s; Wall: %6s] Waiting for teleoperated..." % (\
                        str(autonomous_time),
                        str(arm_in_position),
                        str(wall_distance)
                    ))
            
                drive.ArcadeDrive( 0, 0, False )
                
            return
            
        
        # Is arm at height?
        if arm_in_position:
            
            # Yes! -> In deploy range?
            if wall_distance <= LT_DEPLOY_RANGE:
                
                # Is the middle sensor on?
                if autonomous_time is not None or self.middle_tracker.Get():
                
                    # Turn drive motors off
                    drive.ArcadeDrive(0, 0, False)
                
                    # Yes! -> Deploy tube
                    arm.manual_thump_control( 0.0 )
                    arm.deploy_tube()
                    
                    if self.timer.should_print(3):
                        print("[auton: %4s; Arm: %5s; Wall: %6s] DONE: deploying tube" % (\
                            str(autonomous_time),
                            str(arm_in_position),
                            str(wall_distance)
                        ))
                        
                # No! -> Line tracking
                else:
                
                    if self.timer.should_print(4):
                        print("[auton: %4s; Arm: %5s; Wall: %6s] line tracking in deploy range" % (\
                            str(autonomous_time),
                            str(arm_in_position),
                            str(wall_distance)
                        ))
                
                    self.line_tracking_drive(drive)
                    
            # No! -> Set speed, then...
            else:
                if self.timer.should_print(5):
                    print("[auton: %4s; Arm: %5s; Wall: %6s] arm ready, not close enough to deploy" % (\
                        str(autonomous_time),
                        str(arm_in_position),
                        str(wall_distance)
                    ))
            
                # Line tracking
                self.line_tracking_drive(drive)
                
                
        
        # No! -> Are we too close?
        elif wall_distance <= LT_TOO_CLOSE:
    
            if self.timer.should_print(6):
                print("[auton: %4s; Arm: %5s; Wall: %6s] TOO CLOSE: Waiting for arm to finish deploying"% (\
                    str(autonomous_time),
                    str(arm_in_position),
                    str(wall_distance)
                ))
    
            # Yes! -> Stop motors
            drive.ArcadeDrive(0, 0, False)
        
        # No! -> Line tracking
        else:
        
            if self.timer.should_print(7):
                print("[auton: %4s; Arm: %5s; Wall: %6s] line tracking, arm not in position" % (\
                    str(autonomous_time),
                    str(arm_in_position),
                    str(wall_distance)
                ))
        
            self.line_tracking_drive(drive)
            
