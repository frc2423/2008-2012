import wpilib

''' Sample line tracking class for FIRST 2011 Competition
 * Jumpers on driver station digital I/O pins select the operating mode:
 * The Driver Station digital input 1 select whether the code tracks the straight
 * line or the forked line. Driver station digital input 2 selects whether the
 * code takes the left or right fork. You can set these inputs using jumpers on
 * the USB I/O module or in the driver station I/O Configuration pane (if there
 * is no Digital I/O module installed.
 *
 * Since there is the fork to contend with, the code tracks the edge of the line
 * using a technique similar to that used with a single-sensor Lego robot.
 *
 * This code worked on a simple bot built from the 2011 kit chassis and weighted
 * to behave similarly to complete robot complete with scoring mechanisms. Your
 * own robot will likely be geared differently, the CG will be different, the
 * wheels may be different - so expect to tune the program for your own robot
 * configuration. The two places to do tuning are:
 *
 * defaultSteeringGain - this is the amount of turning correction applied
 * forkProfile & straightProfile - these are power profiles applied at various
 *	times (one power setting / second of travel) as the robot moves towards
 *	the wall.
 *
 * In addition: this program uses dead reckoning - that is it drives at various
 * power settings so it will slow down and stop at the end of the line. This is
 * highly dependent on robot weight, wheel choice, and battery voltage. It will
 * behave differently on a fully charged vs. partially charged battery.
 *
 * To overcome these limitations, you should investigate the use of feedback to
 * have power/wheel independent control of the program. Examples of feedback
 * include using wheel encoders to measure the distance traveled or some kind of
 * rangefinder to determine the distance to the wall. With these sensors installed
 * your program can know precisely how far from the wall it is, and set the speeds
 * accordingly. '''

class RobotDemo(wpilib.SimpleRobot):

    def __init__(self):
        ''' RobotDemo constructor
        This code creates instances of the objects and sets up the driving directions
        for the RobotDrive object. Our robot was geared such that we had to invert
        each of them motor outputs. You may not need to do that '''
        self.drive = wpilib.RobotDrive(1,2) # robot drive base object
        self.drive.SetInvertedMotor(wpilib.RobotDrive.kFrontRightMotor)
        self.drive.SetInvertedMotor(wpilib.RobotDrive.kFrontLeftMotor)
        self.drive.SetInvertedMotor(wpilib.RobotDrive.kRearRightMotor)
        self.drive.SetInvertedMotor(wpilib.RobotDrive.kRearLeftMotor)
        self.drive.SetExpiration(15)
        self.left = wpilib.DigitalInput(1)  # digital inputs for line tracking sensors
        self.middle = wpilib.DigitalInput(2)
        self.right = wpilib.DigitalInput(3)
        self.ds = wpilib.DriverStation.GetInstance()    # driver station object for getting selections
    
	 # This function is called once each time the robot enters autonomous mode.
    def Autonomous(self):
        defaultSteeringGain = 0.65  # default value for steering gain
        
        previousValue =  0
        
        # the power profiles for the straight and forked robot path. They are
		# different to let the robot drive more slowly as the robot approaches
		# the fork on the forked line case.
        forkProfile = [ 0.70, 0.70, 0.55, 0.60, 0.60, 0.50, 0.40, 0.0 ]
        straightProfile = [ 0.70, 0.70, 0.60, 0.60, 0.35, 0.35, 0.35, 0.0 ]
        
        # set the straight vs forked path variables as read from the DS digital
		# inputs or the I/O Setup panel on the driver station.
        straightLine = self.ds.GetDigitaIn(1)
        
        # select the power profile
        if straightLine:
            powerProfile = straightProfile
        else:
            powerProfile = forkProfile
        
        if straightLine:
            stopTime = 2.0
        else:
            stopTime = 4.0
        
        goLeft = not self.ds.GetDigitalIn(2) and not straightLine
        
        print("straightLine %d" % (straightLine) )
        print("goingLeft %d" % (goLeft) )
        
        atCross = False         # true when robot has reached end
        
        # set up timer for 8 second max driving time and use the timer to
		# pick values from the power profile arrays
        timer = wpilib.Timer()
        timer.Start
        timer.Reset
        
        oldTimeInSeconds = -1
        
        # loop until either we hit the "T" at the end or 8 seconds has
		# elapsed. The time to the end should be less than 7 seconds
		# for either path.
        time = timer.Get()
        while time < 8.0 and not atCross:
            timeInSeconds = time
            
            if left.Get():        # read the line tracking sensors
                leftValue = 1
            else:
                leftValue = 0
            
            if middle.Get():
                middleValue = 1
            else:
                middleValue = 0
                
            if right.Get():
                rightValue = 1
            else:
                rightValue = 0
                
            # compute the single value from the 3 sensors. Notice that the bits
		    # for the outside sensors are flipped depending on left or right
		    # fork. Also the sign of the steering direction is different for left/right.
            if goLeft:
                binaryValue = leftValue * 4 + middleValue * 2 + rightValue
                steeringGain = -defaultSteeringGain
            else:
                binaryValue = rightValue * 4 + middleValue * 2 + leftValue
                steeringGain = defaultSteeringGain
                
            speed = powerProfile[timeInSeconds]     # speed value for this time
            turn = 0                                # default to no turn
            
            if binaryValue == 1:
                turn = 0                   # just the outside sensor - drive straight
            elif binaryValue == 7:      # all sensors - maybe at the "T"
                if time > stopTime:
                    atCross = True
                    speed = 0
            elif binaryValue == 0:      # no sensors - apply previous correction
                if previousValue == 0 or previousValue == 1:
                    turn = steeringGain
                else:
                    turn = -steeringGain
            else:                          # anything else, steer back to the line
                turn = -steeringGain
                
            # useful debugging output for tuning your power profile and steering gain
            if binaryValue != previousValue:
                print("Time: %2.2f sensor: %d speed: %1.2f turn: %1.2f atCross: %d\n" % (time, binaryValue, speed, turn, atCross))
            
            # move the robot forward
            self.drive.ArcadeDrive(speed, turn)
            
            if binaryValue != 0:
                previousValue = binaryValue
            
            oldTimeInSeconds = timeInSeconds
            
            wpilib.Wait(0.01)

            time = timer.Get() # this is the bottom of the loop!
        
        # stop driving when finished
        self.drive.ArcadeDrive(0.0, 0.0)
        
    ''' This function is called once each time the robot enters operator control. '''
    def OperatorControl(self):
        # supply your own teleop code here
        pass
        
def run():
    robot = MyRobot()
    robot.StartCompetition()
